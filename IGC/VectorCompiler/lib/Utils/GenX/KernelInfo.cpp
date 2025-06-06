/*========================== begin_copyright_notice ============================

Copyright (C) 2021-2024 Intel Corporation

SPDX-License-Identifier: MIT

============================= end_copyright_notice ===========================*/

#include "vc/Utils/GenX/KernelInfo.h"

#include "llvmWrapper/IR/Function.h"

#include <llvm/Support/CommandLine.h>

using namespace llvm;
using namespace vc;

static cl::opt<bool> UseUpper16Lanes("vc-use-upper16-lanes", cl::init(true),
                                     cl::Hidden,
                                     cl::desc("Use upper 16 SIMD lanes"));

static MDNode *findNode(const Function &F, StringRef KernelsMDName,
                        unsigned KernelRefOp, unsigned MustExceed) {
  NamedMDNode *Named = F.getParent()->getNamedMetadata(KernelsMDName);
  // It's expected that in any case internal and external metadata nodes have
  // already been created by createInternalMD() or vc-intrinsics.
  if (!Named)
    return nullptr;
  auto Res = std::find_if(
      Named->op_begin(), Named->op_end(),
      [&F, KernelRefOp, MustExceed](const MDNode *InternalMD) {
        return InternalMD->getNumOperands() >= MustExceed &&
               &F == getValueAsMetadata(InternalMD->getOperand(KernelRefOp));
      });
  return Res != Named->op_end() ? *Res : nullptr;
}

static MDNode *extendInternalNode(MDNode *Node) {
  using namespace vc::internal;
  if (Node->getNumOperands() == KernelMDOp::LastOptional)
    return Node;

  IGC_ASSERT_MESSAGE(Node->getNumOperands() == KernelMDOp::Last,
                     "Unexpected number of operands in internal node");
  SmallVector<Metadata *, KernelMDOp::LastOptional> NewNode(Node->op_begin(),
                                                            Node->op_end());
  NewNode.resize(KernelMDOp::LastOptional, nullptr);

  auto &Ctx = Node->getContext();
  auto *Ty = Type::getInt32Ty(Ctx);
  auto *CZero = ConstantInt::get(Ty, 0);
  NewNode[KernelMDOp::IndirectCount] = ValueAsMetadata::get(CZero);

  auto *NewMD = MDNode::get(Node->getContext(), NewNode);

  auto *F = cast<Function>(
      getValueAsMetadata(Node->getOperand(KernelMDOp::FunctionRef)));
  auto *M = F->getParent();
  auto *KernelMDs = M->getOrInsertNamedMetadata(FunctionMD::GenXKernelInternal);

  for (unsigned I = 0; I < KernelMDs->getNumOperands(); ++I) {
    if (KernelMDs->getOperand(I) == Node) {
      KernelMDs->setOperand(I, NewMD);
      break;
    }
  }

  return NewMD;
}

static MDNode *findInternalNode(const Function &F) {
  using namespace vc::internal;
  auto *Node = findNode(F, FunctionMD::GenXKernelInternal,
                        KernelMDOp::FunctionRef, KernelMDOp::Last);
  return Node ? extendInternalNode(Node) : nullptr;
}

static MDNode *findExternalNode(const Function &F) {
  return findNode(F, genx::FunctionMD::GenXKernels,
                  genx::KernelMDOp::FunctionRef,
                  genx::KernelMDOp::ArgTypeDescs);
}

static MDNode *findSpirvExecutionModeNode(const Function &F) {
  constexpr unsigned FunctionRef = 0;
  constexpr unsigned MinNumOps = 3;
  return findNode(F, "spirv.ExecutionMode", FunctionRef, MinNumOps);
};

void vc::internal::createInternalMD(Function &F) {
  IGC_ASSERT_MESSAGE(!findInternalNode(F),
                     "Internal node has already been created!");

  auto &Ctx = F.getContext();
  auto *M = F.getParent();

  // Create nullptr values by default.
  SmallVector<Metadata *, KernelMDOp::LastOptional> KernelInternalMD(
      KernelMDOp::LastOptional, nullptr);
  KernelInternalMD[KernelMDOp::FunctionRef] = ValueAsMetadata::get(&F);

  auto *Ty = Type::getInt32Ty(Ctx);
  auto *CZero = ConstantInt::get(Ty, 0);
  KernelInternalMD[KernelMDOp::IndirectCount] = ValueAsMetadata::get(CZero);

  auto *InternalNode = MDNode::get(Ctx, KernelInternalMD);
  auto *KernelMDs = M->getOrInsertNamedMetadata(FunctionMD::GenXKernelInternal);
  KernelMDs->addOperand(InternalNode);
}

void vc::internal::replaceInternalFunctionRef(const Function &From,
                                              Function &To) {
  auto *Node = findInternalNode(From);
  IGC_ASSERT_MESSAGE(Node, "Replacement was called for non existing in kernel "
                           "internal metadata function");
  Node->replaceOperandWith(internal::KernelMDOp::FunctionRef,
                           ValueAsMetadata::get(&To));
}

void vc::replaceFunctionRefMD(const Function &From, Function &To) {
  Module *M = To.getParent();
  NamedMDNode *Named = M->getNamedMetadata(genx::FunctionMD::GenXKernels);
  IGC_ASSERT(Named);

  auto Res =
      std::find_if(Named->op_begin(), Named->op_end(), [&From](MDNode *Node) {
        auto *NodeVal = cast<ValueAsMetadata>(
                            Node->getOperand(genx::KernelMDOp::FunctionRef))
                            ->getValue();
        auto *F = cast<Function>(NodeVal);
        return &From == F;
      });
  IGC_ASSERT_MESSAGE(Res != Named->op_end(),
                     "Cannot find MD for 'From' function");

  MDNode *FromNode = *Res;
  FromNode->replaceOperandWith(genx::KernelMDOp::FunctionRef,
                               ValueAsMetadata::get(&To));

  internal::replaceInternalFunctionRef(From, To);
}

template <typename RetTy = unsigned>
static RetTy extractConstantIntMD(const MDOperand &Op) {
  const auto *V = getValueAsMetadata<ConstantInt>(Op);
  IGC_ASSERT_MESSAGE(V, "Unexpected null value in metadata");
  return static_cast<RetTy>(V->getZExtValue());
}

template <typename RetTy = unsigned>
static RetTy extractConstantIntMD(const Metadata &Op) {
  const auto *V = getValueAsMetadata<ConstantInt>(&Op);
  IGC_ASSERT_MESSAGE(V, "Unexpected null value in metadata");
  return static_cast<RetTy>(V->getZExtValue());
}

template <typename Cont>
static void extractConstantsFromMDNode(const MDNode *N, Cont &C) {
  if (!N)
    return;
  using ValTy = typename Cont::value_type;
  std::transform(
      N->op_begin(), N->op_end(), std::back_inserter(C),
      [](const MDOperand &Op) { return extractConstantIntMD<ValTy>(Op); });
}

static ImplicitLinearizationInfo
extractImplicitLinearizationArg(const Function &F,
                                const MDOperand &ImplicitArg) {
  auto *MD = cast<MDNode>(ImplicitArg.get());
  IGC_ASSERT(MD->getNumOperands() == internal::LinearizationMDOp::Last);
  Constant *ArgNoValue =
      cast<ConstantAsMetadata>(
          MD->getOperand(internal::LinearizationMDOp::Argument).get())
          ->getValue();
  unsigned ArgNo = cast<ConstantInt>(ArgNoValue)->getZExtValue();
  Argument *Arg = IGCLLVM::getArg(F, ArgNo);
  auto *OffsetMD = cast<ConstantAsMetadata>(
      MD->getOperand(internal::LinearizationMDOp::Offset).get());
  return ImplicitLinearizationInfo{Arg,
                                   cast<ConstantInt>(OffsetMD->getValue())};
}

static ArgToImplicitLinearization::value_type
extractArgLinearization(const Function &F, const MDOperand &MDOp) {
  auto *ArgLinearizationMD = cast<MDNode>(MDOp.get());
  IGC_ASSERT(ArgLinearizationMD->getNumOperands() ==
             internal::ArgLinearizationMDOp::Last);
  Constant *ExplicitArgNo =
      cast<ConstantAsMetadata>(
          ArgLinearizationMD
              ->getOperand(internal::ArgLinearizationMDOp::Explicit)
              .get())
          ->getValue();
  Argument *ExplicitArg =
      IGCLLVM::getArg(F, cast<ConstantInt>(ExplicitArgNo)->getZExtValue());
  auto *LinMD = cast<MDNode>(
      ArgLinearizationMD
          ->getOperand(internal::ArgLinearizationMDOp::Linearization)
          .get());
  LinearizedArgInfo Info;
  std::transform(LinMD->op_begin(), LinMD->op_end(), std::back_inserter(Info),
                 [&F](const MDOperand &ImplicitArg) {
                   return extractImplicitLinearizationArg(F, ImplicitArg);
                 });
  return std::make_pair(ExplicitArg, std::move(Info));
}

static ArgToImplicitLinearization
extractLinearizationMD(const Function &F, const MDNode *LinearizationNode) {
  IGC_ASSERT(LinearizationNode);
  ArgToImplicitLinearization Linearization;
  std::transform(
      LinearizationNode->op_begin(), LinearizationNode->op_end(),
      std::inserter(Linearization, Linearization.end()),
      [&F](const MDOperand &MDOp) { return extractArgLinearization(F, MDOp); });
  return Linearization;
}

vc::KernelMetadata::KernelMetadata(const Function *F) {
  if (!vc::isKernel(F))
    return;

  ExternalNode = findExternalNode(*F);
  if (!ExternalNode)
    return;

  // ExternalNode is the metadata node for F, and it has the required number of
  // operands.
  this->F = F;
  IsKernel = true;
  if (MDString *MDS =
          dyn_cast<MDString>(ExternalNode->getOperand(genx::KernelMDOp::Name)))
    Name = MDS->getString();
  if (ConstantInt *Sz = getValueAsMetadata<ConstantInt>(
          ExternalNode->getOperand(genx::KernelMDOp::SLMSize)))
    SLMSize = Sz->getZExtValue();
  if (ExternalNode->getNumOperands() > genx::KernelMDOp::NBarrierCnt)
    if (ConstantInt *Sz = getValueAsMetadata<ConstantInt>(
            ExternalNode->getOperand(genx::KernelMDOp::NBarrierCnt)))
      NBarrierCnt = Sz->getZExtValue();
  // Build the argument kinds and offsets arrays that should correspond to the
  // function arguments (both explicit and implicit)
  MDNode *KindsNode =
      dyn_cast<MDNode>(ExternalNode->getOperand(genx::KernelMDOp::ArgKinds));
  MDNode *OffsetsNode =
      dyn_cast<MDNode>(ExternalNode->getOperand(genx::KernelMDOp::ArgOffsets));
  MDNode *InputOutputKinds =
      dyn_cast<MDNode>(ExternalNode->getOperand(genx::KernelMDOp::ArgIOKinds));
  MDNode *ArgDescNode = dyn_cast<MDNode>(
      ExternalNode->getOperand(genx::KernelMDOp::ArgTypeDescs));

  MDNode *IndexesNode = nullptr;
  MDNode *OffsetInArgsNode = nullptr;
  MDNode *LinearizationNode = nullptr;
  MDNode *BTIndicesNode = nullptr;
  InternalNode = findInternalNode(*F);
  IGC_ASSERT_MESSAGE(InternalNode,
                     "Internal node is expected to have already been created!");

  IndexesNode = cast_or_null<MDNode>(
      InternalNode->getOperand(internal::KernelMDOp::ArgIndexes));
  OffsetInArgsNode = cast_or_null<MDNode>(
      InternalNode->getOperand(internal::KernelMDOp::OffsetInArgs));
  LinearizationNode = cast_or_null<MDNode>(
      InternalNode->getOperand(internal::KernelMDOp::LinearizationArgs));
  BTIndicesNode = cast_or_null<MDNode>(
      InternalNode->getOperand(internal::KernelMDOp::BTIndices));

  auto &MD = InternalNode->getOperand(internal::KernelMDOp::IndirectCount);
  if (auto *Count = getValueAsMetadata<ConstantInt>(MD))
    IndirectCount = Count->getZExtValue();

  IGC_ASSERT(KindsNode);

  // These should have the same number of operands if they exist.
  IGC_ASSERT(!OffsetsNode ||
             KindsNode->getNumOperands() == OffsetsNode->getNumOperands());
  IGC_ASSERT(!OffsetInArgsNode ||
             KindsNode->getNumOperands() == OffsetInArgsNode->getNumOperands());
  IGC_ASSERT(!IndexesNode ||
             KindsNode->getNumOperands() == IndexesNode->getNumOperands());
  IGC_ASSERT(!BTIndicesNode ||
             KindsNode->getNumOperands() == BTIndicesNode->getNumOperands());

  extractConstantsFromMDNode(KindsNode, ArgKinds);
  extractConstantsFromMDNode(OffsetsNode, ArgOffsets);
  extractConstantsFromMDNode(OffsetInArgsNode, OffsetInArgs);
  extractConstantsFromMDNode(IndexesNode, ArgIndexes);
  extractConstantsFromMDNode(BTIndicesNode, BTIs);

  IGC_ASSERT(InputOutputKinds);
  IGC_ASSERT(KindsNode->getNumOperands() >= InputOutputKinds->getNumOperands());
  extractConstantsFromMDNode(InputOutputKinds, ArgIOKinds);

  IGC_ASSERT(ArgDescNode);
  for (unsigned i = 0, e = ArgDescNode->getNumOperands(); i < e; ++i) {
    MDString *MDS = dyn_cast<MDString>(ArgDescNode->getOperand(i));
    IGC_ASSERT(MDS);
    ArgTypeDescs.push_back(MDS->getString());
  }
  if (LinearizationNode)
    Linearization = extractLinearizationMD(*F, LinearizationNode);

  MDNode *SpirvExecutionMode = findSpirvExecutionModeNode(*F);
  if (!SpirvExecutionMode)
    return;
  parseExecutionMode(SpirvExecutionMode);
}

static MDNode *createArgLinearizationMD(const ImplicitLinearizationInfo &Info) {
  auto &Ctx = Info.Arg->getContext();
  auto *I32Ty = Type::getInt32Ty(Ctx);
  Metadata *ArgMD =
      ConstantAsMetadata::get(ConstantInt::get(I32Ty, Info.Arg->getArgNo()));
  Metadata *OffsetMD = ConstantAsMetadata::get(Info.Offset);
  return MDNode::get(Ctx, {ArgMD, OffsetMD});
}

void vc::KernelMetadata::updateLinearizationMD(
    ArgToImplicitLinearization &&Lin) {
  Linearization = std::move(Lin);

  std::vector<Metadata *> LinMDs;
  LinMDs.reserve(Linearization.size());
  auto &Ctx = F->getContext();
  for (const auto &ArgLin : Linearization) {
    std::vector<Metadata *> ArgLinMDs;
    ArgLinMDs.reserve(ArgLin.second.size());
    std::transform(ArgLin.second.begin(), ArgLin.second.end(),
                   std::back_inserter(ArgLinMDs), createArgLinearizationMD);
    auto *I32Ty = Type::getInt32Ty(Ctx);
    Metadata *ExplicitArgMD = ConstantAsMetadata::get(
        ConstantInt::get(I32Ty, ArgLin.first->getArgNo()));
    Metadata *ExplicitArgLinMD = MDNode::get(Ctx, ArgLinMDs);
    LinMDs.push_back(MDNode::get(Ctx, {ExplicitArgMD, ExplicitArgLinMD}));
  }
  InternalNode->replaceOperandWith(internal::KernelMDOp::LinearizationArgs,
                                   MDNode::get(Ctx, LinMDs));
}

template <typename InputIt>
void vc::KernelMetadata::updateArgsMD(InputIt Begin, InputIt End, MDNode *Node,
                                      unsigned NodeOpNo) const {
  IGC_ASSERT(F);
  IGC_ASSERT(Node);
  IGC_ASSERT_MESSAGE(std::distance(Begin, End) == getNumArgs(),
                     "Mismatch between metadata for kernel and number of args");
  IGC_ASSERT(Node->getNumOperands() > NodeOpNo);
  auto &Ctx = F->getContext();
  auto *I32Ty = Type::getInt32Ty(Ctx);
  SmallVector<Metadata *, 8> NewMD;
  std::transform(Begin, End, std::back_inserter(NewMD), [I32Ty](auto Value) {
    return ValueAsMetadata::getConstant(ConstantInt::get(I32Ty, Value));
  });
  MDNode *NewNode = MDNode::get(Ctx, NewMD);
  Node->replaceOperandWith(NodeOpNo, NewNode);
}

void vc::KernelMetadata::updateArgOffsetsMD(
    SmallVectorImpl<unsigned> &&Offsets) {
  ArgOffsets = std::move(Offsets);
  updateArgsMD(ArgOffsets.begin(), ArgOffsets.end(), ExternalNode,
               genx::KernelMDOp::ArgOffsets);
}

void vc::KernelMetadata::updateArgKindsMD(SmallVectorImpl<unsigned> &&Kinds) {
  ArgKinds = std::move(Kinds);
  updateArgsMD(ArgKinds.begin(), ArgKinds.end(), ExternalNode,
               genx::KernelMDOp::ArgKinds);
}

void vc::KernelMetadata::updateArgIndexesMD(
    SmallVectorImpl<unsigned> &&Indexes) {
  ArgIndexes = std::move(Indexes);
  updateArgsMD(ArgIndexes.begin(), ArgIndexes.end(), InternalNode,
               internal::KernelMDOp::ArgIndexes);
}

void vc::KernelMetadata::updateArgTypeDescsMD(
    SmallVectorImpl<StringRef> &&Descs) {
  ArgTypeDescs = std::move(Descs);
  auto &Ctx = F->getContext();
  SmallVector<Metadata *, 4> DescMDs;
  llvm::transform(ArgTypeDescs, std::back_inserter(DescMDs),
                  [&Ctx](StringRef Desc) { return MDString::get(Ctx, Desc); });
  ExternalNode->replaceOperandWith(genx::KernelMDOp::ArgTypeDescs,
                                   MDNode::get(Ctx, DescMDs));
}

void vc::KernelMetadata::updateOffsetInArgsMD(
    SmallVectorImpl<unsigned> &&Offsets) {
  OffsetInArgs = std::move(Offsets);
  updateArgsMD(OffsetInArgs.begin(), OffsetInArgs.end(), InternalNode,
               internal::KernelMDOp::OffsetInArgs);
}

void vc::KernelMetadata::updateBTIndicesMD(std::vector<int> &&BTIndices) {
  BTIs = std::move(BTIndices);
  updateArgsMD(BTIs.begin(), BTIs.end(), InternalNode,
               internal::KernelMDOp::BTIndices);
}

void vc::KernelMetadata::updateSLMSizeMD(unsigned Size) {
  SLMSize = Size;
  auto *Ty = Type::getInt32Ty(F->getContext());
  auto *C = ConstantInt::get(Ty, SLMSize);
  ExternalNode->replaceOperandWith(genx::KernelMDOp::SLMSize,
                                   ValueAsMetadata::get(C));
}

void vc::KernelMetadata::updateIndirectCountMD(unsigned Count) {
  IndirectCount = Count;
  auto *Ty = Type::getInt32Ty(F->getContext());
  auto *C = ConstantInt::get(Ty, Count);
  auto *MD = ValueAsMetadata::get(C);
  InternalNode->replaceOperandWith(internal::KernelMDOp::IndirectCount, MD);
}

void vc::KernelMetadata::parseExecutionMode(MDNode *SpirvExecutionMode) {
  IGC_ASSERT(SpirvExecutionMode->getNumOperands() >= 3);

  auto &EMode = SpirvExecutionMode->getOperand(1);
  auto &EModeVal = SpirvExecutionMode->getOperand(2);
  auto EModeId = static_cast<ExecutionMode>(extractConstantIntMD(EMode));
  switch (EModeId) {
  case ExecutionMode::MaximumRegistersINTEL:
    GRFSize = extractConstantIntMD(*(EModeVal.get()));
    return;
  case ExecutionMode::MaximumRegistersIdINTEL: {
    auto *GRFSizeNode = cast<MDNode>(EModeVal.get());
    GRFSize = extractConstantIntMD(*(GRFSizeNode->getOperand(0)));
    return;
  }
  case ExecutionMode::NamedMaximumRegistersINTEL: {
    auto *NamedExecMode = cast<MDString>(EModeVal.get());
    IGC_ASSERT_EXIT_MESSAGE(!NamedExecMode->getString().compare("AutoINTEL"),
                            "Unhandled NamedMaximumRegisters value");
    GRFSize = 0;
    return;
  }
  }
  IGC_ASSERT_EXIT_MESSAGE(0, "Unhandled execution mode!");
}

bool vc::hasKernel(const Module &M) {
  NamedMDNode *KernelsMD = M.getNamedMetadata(genx::FunctionMD::GenXKernels);
  if (!KernelsMD)
    return false;
  return KernelsMD->getNumOperands();
}

bool vc::canUseSIMD32(const Module &M, bool HasFusedEU) {
  if (!UseUpper16Lanes)
    return false;
  if (!HasFusedEU)
    return true;
  // FIXME: Non-optimal solution. FGs info or some stackcalls-related analysis
  // will be useful here.
  bool HasStackCalls = llvm::any_of(M.functions(), [](const Function &F) {
    return vc::requiresStackCall(F);
  });
  return !HasStackCalls;
}

const llvm::Argument &vc::getImplicitArg(const llvm::Function &Kernel,
                                         KernelMetadata::ImpValue ImplArgID) {
  IGC_ASSERT_MESSAGE(vc::isKernel(Kernel), "a kernel was expected");
  vc::KernelMetadata KM{&Kernel};
  auto *ImplArgIt = std::find_if(
      Kernel.arg_begin(), Kernel.arg_end(),
      [&KM, ImplArgID](const Argument &Arg) {
        return vc::isImplicitArgKind(KM.getArgKind(Arg.getArgNo()), ImplArgID);
      });
  IGC_ASSERT_MESSAGE(ImplArgIt != Kernel.arg_end(),
                     "the requested implicit arg wasn't found");
  return *ImplArgIt;
}
