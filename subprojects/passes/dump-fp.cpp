#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/ADT/StringSet.h"
#include "llvm/ADT/SmallSet.h"
#include "llvm/ADT/SmallVector.h"
#include "llvm/Transforms/Utils/Cloning.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/ADT/SetVector.h"
#include "llvm/Support/CommandLine.h"
#include <llvm/Support/raw_ostream.h>
#include <system_error>
#include <assert.h>

static llvm::cl::opt<std::string> Output("out-float", llvm::cl::desc("Specify output filename"), llvm::cl::value_desc("filename"));
llvm::cl::opt<bool> SkipGvec("skip-gvec", llvm::cl::desc("Skip gvec output of functions"), llvm::cl::value_desc("bool"), llvm::cl::init(false));

llvm::StringSet<> FloatNames = {
    "float_raise",
    "float16_squash_input_denormal",
    "float32_squash_input_denormal",
    "float64_squash_input_denormal",
    "bfloat16_squash_input_denormal",
    "int16_to_float16_scalbn",
    "int32_to_float16_scalbn",
    "int64_to_float16_scalbn",
    "uint16_to_float16_scalbn",
    "uint32_to_float16_scalbn",
    "uint64_to_float16_scalbn",
    "int8_to_float16",
    "int16_to_float16",
    "int32_to_float16",
    "int64_to_float16",
    "uint8_to_float16",
    "uint16_to_float16",
    "uint32_to_float16",
    "uint64_to_float16",
    "int16_to_float32_scalbn",
    "int32_to_float32_scalbn",
    "int64_to_float32_scalbn",
    "uint16_to_float32_scalbn",
    "uint32_to_float32_scalbn",
    "uint64_to_float32_scalbn",
    "int16_to_float32",
    "int32_to_float32",
    "int64_to_float32",
    "uint16_to_float32",
    "uint32_to_float32",
    "uint64_to_float32",
    "int16_to_float64_scalbn",
    "int32_to_float64_scalbn",
    "int64_to_float64_scalbn",
    "uint16_to_float64_scalbn",
    "uint32_to_float64_scalbn",
    "uint64_to_float64_scalbn",
    "int16_to_float64",
    "int32_to_float64",
    "int64_to_float64",
    "uint16_to_float64",
    "uint32_to_float64",
    "uint64_to_float64",
    "int32_to_floatx80",
    "int64_to_floatx80",
    "int32_to_float128",
    "int64_to_float128",
    "int128_to_float128",
    "uint64_to_float128",
    "uint128_to_float128",
    "float32_to_float16",
    "float16_to_float32",
    "float64_to_float16",
    "float16_to_float64",
    "float16_to_int8_scalbn",
    "float16_to_int16_scalbn",
    "float16_to_int32_scalbn",
    "float16_to_int64_scalbn",
    "float16_to_int8",
    "float16_to_int16",
    "float16_to_int32",
    "float16_to_int64",
    "float16_to_int16_round_to_zero",
    "float16_to_int32_round_to_zero",
    "float16_to_int64_round_to_zero",
    "float16_to_uint8_scalbn",
    "float16_to_uint16_scalbn",
    "float16_to_uint32_scalbn",
    "float16_to_uint64_scalbn",
    "float16_to_uint8",
    "float16_to_uint16",
    "float16_to_uint32",
    "float16_to_uint64",
    "float16_to_uint16_round_to_zero",
    "float16_to_uint32_round_to_zero",
    "float16_to_uint64_round_to_zero",
    "float16_round_to_int",
    "float16_add",
    "float16_sub",
    "float16_mul",
    "float16_muladd",
    "float16_div",
    "float16_scalbn",
    "float16_min",
    "float16_max",
    "float16_minnum",
    "float16_maxnum",
    "float16_minnummag",
    "float16_maxnummag",
    "float16_minimum_number",
    "float16_maximum_number",
    "float16_sqrt",
    "float16_compare",
    "float16_compare_quiet",
    "float16_is_quiet_nan",
    "float16_is_signaling_nan",
    "float16_silence_nan",
    "float16_is_any_nan",
    "float16_is_neg",
    "float16_is_infinity",
    "float16_is_zero",
    "float16_is_zero_or_denormal",
    "float16_is_normal",
    "float16_abs",
    "float16_chs",
    "float16_set_sign",
    "float16_eq",
    "float16_le",
    "float16_lt",
    "float16_unordered",
    "float16_eq_quiet",
    "float16_le_quiet",
    "float16_lt_quiet",
    "float16_unordered_quiet",
    "bfloat16_round_to_int",
    "float32_to_bfloat16",
    "bfloat16_to_float32",
    "float64_to_bfloat16",
    "bfloat16_to_float64",
    "bfloat16_to_int8_scalbn",
    "bfloat16_to_int16_scalbn",
    "bfloat16_to_int32_scalbn",
    "bfloat16_to_int64_scalbn",
    "bfloat16_to_int8",
    "bfloat16_to_int16",
    "bfloat16_to_int32",
    "bfloat16_to_int64",
    "bfloat16_to_int8_round_to_zero",
    "bfloat16_to_int16_round_to_zero",
    "bfloat16_to_int32_round_to_zero",
    "bfloat16_to_int64_round_to_zero",
    "bfloat16_to_uint8_scalbn",
    "bfloat16_to_uint16_scalbn",
    "bfloat16_to_uint32_scalbn",
    "bfloat16_to_uint64_scalbn",
    "bfloat16_to_uint8",
    "bfloat16_to_uint16",
    "bfloat16_to_uint32",
    "bfloat16_to_uint64",
    "bfloat16_to_uint8_round_to_zero",
    "bfloat16_to_uint16_round_to_zero",
    "bfloat16_to_uint32_round_to_zero",
    "bfloat16_to_uint64_round_to_zero",
    "int8_to_bfloat16_scalbn",
    "int16_to_bfloat16_scalbn",
    "int32_to_bfloat16_scalbn",
    "int64_to_bfloat16_scalbn",
    "uint8_to_bfloat16_scalbn",
    "uint16_to_bfloat16_scalbn",
    "uint32_to_bfloat16_scalbn",
    "uint64_to_bfloat16_scalbn",
    "int8_to_bfloat16",
    "int16_to_bfloat16",
    "int32_to_bfloat16",
    "int64_to_bfloat16",
    "uint8_to_bfloat16",
    "uint16_to_bfloat16",
    "uint32_to_bfloat16",
    "uint64_to_bfloat16",
    "bfloat16_add",
    "bfloat16_sub",
    "bfloat16_mul",
    "bfloat16_div",
    "bfloat16_muladd",
    "bfloat16_scalbn",
    "bfloat16_min",
    "bfloat16_max",
    "bfloat16_minnum",
    "bfloat16_maxnum",
    "bfloat16_minnummag",
    "bfloat16_maxnummag",
    "bfloat16_minimum_number",
    "bfloat16_maximum_number",
    "bfloat16_sqrt",
    "bfloat16_compare",
    "bfloat16_compare_quiet",
    "bfloat16_is_quiet_nan",
    "bfloat16_is_signaling_nan",
    "bfloat16_silence_nan",
    "bfloat16_default_nan",
    "bfloat16_is_any_nan",
    "bfloat16_is_neg",
    "bfloat16_is_infinity",
    "bfloat16_is_zero",
    "bfloat16_is_zero_or_denormal",
    "bfloat16_is_normal",
    "bfloat16_abs",
    "bfloat16_chs",
    "bfloat16_set_sign",
    "bfloat16_eq",
    "bfloat16_le",
    "bfloat16_lt",
    "bfloat16_unordered",
    "bfloat16_eq_quiet",
    "bfloat16_le_quiet",
    "bfloat16_lt_quiet",
    "bfloat16_unordered_quiet",
    "float16_default_nan",
    "float32_to_int16_scalbn",
    "float32_to_int32_scalbn",
    "float32_to_int64_scalbn",
    "float32_to_int16",
    "float32_to_int32",
    "float32_to_int64",
    "float32_to_int16_round_to_zero",
    "float32_to_int32_round_to_zero",
    "float32_to_int64_round_to_zero",
    "float32_to_uint16_scalbn",
    "float32_to_uint32_scalbn",
    "float32_to_uint64_scalbn",
    "float32_to_uint16",
    "float32_to_uint32",
    "float32_to_uint64",
    "float32_to_uint16_round_to_zero",
    "float32_to_uint32_round_to_zero",
    "float32_to_uint64_round_to_zero",
    "float32_to_float64",
    "float32_to_floatx80",
    "float32_to_float128",
    "float32_round_to_int",
    "float32_add",
    "float32_sub",
    "float32_mul",
    "float32_div",
    "float32_rem",
    "float32_muladd",
    "float32_sqrt",
    "float32_exp2",
    "float32_log2",
    "float32_compare",
    "float32_compare_quiet",
    "float32_min",
    "float32_max",
    "float32_minnum",
    "float32_maxnum",
    "float32_minnummag",
    "float32_maxnummag",
    "float32_minimum_number",
    "float32_maximum_number",
    "float32_is_quiet_nan",
    "float32_is_signaling_nan",
    "float32_silence_nan",
    "float32_scalbn",
    "float32_abs",
    "float32_chs",
    "float32_is_infinity",
    "float32_is_neg",
    "float32_is_zero",
    "float32_is_any_nan",
    "float32_is_zero_or_denormal",
    "float32_is_normal",
    "float32_is_denormal",
    "float32_is_zero_or_normal",
    "float32_set_sign",
    "float32_eq",
    "float32_le",
    "float32_lt",
    "float32_unordered",
    "float32_eq_quiet",
    "float32_le_quiet",
    "float32_lt_quiet",
    "float32_unordered_quiet",
    "packFloat32",
    "float32_default_nan",
    "float64_to_int16_scalbn",
    "float64_to_int32_scalbn",
    "float64_to_int64_scalbn",
    "float64_to_int16",
    "float64_to_int32",
    "float64_to_int64",
    "float64_to_int16_round_to_zero",
    "float64_to_int32_round_to_zero",
    "float64_to_int64_round_to_zero",
    "float64_to_int32_modulo",
    "float64_to_int64_modulo",
    "float64_to_uint16_scalbn",
    "float64_to_uint32_scalbn",
    "float64_to_uint64_scalbn",
    "float64_to_uint16",
    "float64_to_uint32",
    "float64_to_uint64",
    "float64_to_uint16_round_to_zero",
    "float64_to_uint32_round_to_zero",
    "float64_to_uint64_round_to_zero",
    "float64_to_float32",
    "float64_to_floatx80",
    "float64_to_float128",
    "float64_round_to_int",
    "float64_add",
    "float64_sub",
    "float64_mul",
    "float64_div",
    "float64_rem",
    "float64_muladd",
    "float64_sqrt",
    "float64_log2",
    "float64_compare",
    "float64_compare_quiet",
    "float64_min",
    "float64_max",
    "float64_minnum",
    "float64_maxnum",
    "float64_minnummag",
    "float64_maxnummag",
    "float64_minimum_number",
    "float64_maximum_number",
    "float64_is_quiet_nan",
    "float64_is_signaling_nan",
    "float64_silence_nan",
    "float64_scalbn",
    "float64_abs",
    "float64_chs",
    "float64_is_infinity",
    "float64_is_neg",
    "float64_is_zero",
    "float64_is_any_nan",
    "float64_is_zero_or_denormal",
    "float64_is_normal",
    "float64_is_denormal",
    "float64_is_zero_or_normal",
    "float64_set_sign",
    "float64_eq",
    "float64_le",
    "float64_lt",
    "float64_unordered",
    "float64_eq_quiet",
    "float64_le_quiet",
    "float64_lt_quiet",
    "float64_unordered_quiet",
    "float64_default_nan",
    "float64r32_add",
    "float64r32_sub",
    "float64r32_mul",
    "float64r32_div",
    "float64r32_muladd",
    "float64r32_sqrt",
    "floatx80_to_int32",
    "floatx80_to_int32_round_to_zero",
    "floatx80_to_int64",
    "floatx80_to_int64_round_to_zero",
    "floatx80_to_float32",
    "floatx80_to_float64",
    "floatx80_to_float128",
    "floatx80_round",
    "floatx80_round_to_int",
    "floatx80_add",
    "floatx80_sub",
    "floatx80_mul",
    "floatx80_div",
    "floatx80_modrem",
    "floatx80_mod",
    "floatx80_rem",
    "floatx80_sqrt",
    "floatx80_compare",
    "floatx80_compare_quiet",
    "floatx80_is_quiet_nan",
    "floatx80_is_signaling_nan",
    "floatx80_silence_nan",
    "floatx80_scalbn",
    "floatx80_abs",
    "floatx80_chs",
    "floatx80_is_infinity",
    "floatx80_is_neg",
    "floatx80_is_zero",
    "floatx80_is_zero_or_denormal",
    "floatx80_is_any_nan",
    "floatx80_eq",
    "floatx80_le",
    "floatx80_lt",
    "floatx80_unordered",
    "floatx80_eq_quiet",
    "floatx80_le_quiet",
    "floatx80_lt_quiet",
    "floatx80_unordered_quiet",
    "floatx80_invalid_encoding",
    "extractFloatx80Frac",
    "extractFloatx80Exp",
    "extractFloatx80Sign",
    "packFloatx80",
    "normalizeFloatx80Subnormal",
    "propagateFloatx80NaN",
    "roundAndPackFloatx80",
    "normalizeRoundAndPackFloatx80",
    "floatx80_default_nan",
    "float128_to_int32",
    "float128_to_int32_round_to_zero",
    "float128_to_int64",
    "float128_to_int128",
    "float128_to_int64_round_to_zero",
    "float128_to_int128_round_to_zero",
    "float128_to_uint64",
    "float128_to_uint128",
    "float128_to_uint64_round_to_zero",
    "float128_to_uint128_round_to_zero",
    "float128_to_uint32",
    "float128_to_uint32_round_to_zero",
    "float128_to_float32",
    "float128_to_float64",
    "float128_to_floatx80",
    "float128_round_to_int",
    "float128_add",
    "float128_sub",
    "float128_mul",
    "float128_muladd",
    "float128_div",
    "float128_rem",
    "float128_sqrt",
    "float128_compare",
    "float128_compare_quiet",
    "float128_min",
    "float128_max",
    "float128_minnum",
    "float128_maxnum",
    "float128_minnummag",
    "float128_maxnummag",
    "float128_minimum_number",
    "float128_maximum_number",
    "float128_is_quiet_nan",
    "float128_is_signaling_nan",
    "float128_silence_nan",
    "float128_scalbn",
    "float128_abs",
    "float128_chs",
    "float128_is_infinity",
    "float128_is_neg",
    "float128_is_zero",
    "float128_is_zero_or_denormal",
    "float128_is_normal",
    "float128_is_denormal",
    "float128_is_any_nan",
    "float128_eq",
    "float128_le",
    "float128_lt",
    "float128_unordered",
    "float128_eq_quiet",
    "float128_le_quiet",
    "float128_lt_quiet",
    "float128_unordered_quiet",
    "float128_default_nan",
};

llvm::StringSet<> OtherNames = {
    "cpu_loop_exit",
    "cpu_loop_exit_restore",
    "cpu_restore_state",
    "cpu_abort",
    "__assert_fail",
    "g_assertion_message_expr",
    "qemu_log",
    "qemu_loglevel_mask",
};

struct StackInfo {
    llvm::SmallVector<llvm::Function *> Stack;
};

struct StacksToInline {
    llvm::Function *F;
    llvm::SetVector<llvm::CallInst *> Stack;
};

class DumpFP : public llvm::PassInfoMixin<DumpFP> {
public:
    llvm::PreservedAnalyses run(llvm::Module &M, llvm::ModuleAnalysisManager &MAM) {

        llvm::SmallVector<StacksToInline> FloatStack;

        for (auto &F : M) {
            if (!F.hasName())
                continue;

            const auto Name = F.getName();
            if (!Name.starts_with("helper_"))
                continue;

            llvm::SmallSet<llvm::Function *, 8> Visited;
            llvm::SmallVector<StackInfo> Q;
            Q.push_back({{&F}});

            llvm::SetVector<llvm::CallInst *> InlineStack;

            while (Q.size() > 0) {
                StackInfo info = Q.back();
                Q.pop_back();
                auto *F = info.Stack.back();

                if (Visited.contains(F)) {
                    continue;
                }
                Visited.insert(F);
                bool HasFloat = false;

                for (auto &BB : *F) {
                    for (auto &I : BB) {
                        auto *Call = llvm::dyn_cast<llvm::CallInst>(&I);
                        if (Call) {
                            auto *CalledF = Call->getCalledFunction();
                            if (CalledF) {

                                if (CalledF->hasName()) {
                                    auto CalledName = CalledF->getName();
                                    auto It = CalledName.find('.');
                                    if (It != llvm::StringRef::npos) {
                                        CalledName = CalledName.substr(0, It);
                                    }
                                    if (FloatNames.contains(CalledName)) {
                                        HasFloat = true;
                                        continue;
                                    } else if (OtherNames.contains(CalledName) or CalledName.starts_with("llvm")) {
                                        continue;
                                    }
                                }

                                Q.push_back({info.Stack});
                                Q.back().Stack.push_back(CalledF);
                                InlineStack.insert(Call);

                            }
                        }
                    }
                }

                if (HasFloat) {
                    FloatStack.push_back({F, InlineStack});
                }

            }

        }

        llvm::SmallSet<llvm::CallInst *, 16> Handled;
        for (auto &Info : FloatStack) {

            for (auto &Call : llvm::reverse(Info.Stack)) {
                if (Handled.contains(Call)) {
                    continue;
                }
                Handled.insert(Call);

                llvm::InlineFunctionInfo IFI;
                auto Res = llvm::InlineFunction(*Call, IFI);
                if (!Res.isSuccess()) {
                    llvm::errs() << "Failed to inline:\n";
                    llvm::errs() << "  call: " << *Call << "\n";
                    llvm::errs() << "  func: " << Call->getParent()->getParent()->getName() << "\n";
                    llvm::errs() << "  reason: " << Res.getFailureReason() << "\n";
                }
            }
        }

        if (!Output.empty()) {
            std::error_code EC;
            llvm::raw_fd_ostream OF(Output.getValue(), EC);
            assert(!EC);

            for (auto &Info : FloatStack) {
                auto *F = Info.F;
                llvm::SmallVector<llvm::Instruction *> InstToErase;
                for (auto &BB : *F) {
                    for (auto &I : BB) {
                        if (I.isDebugOrPseudoInst() or I.isLifetimeStartOrEnd()) {
                            InstToErase.push_back(&I);
                        }
                    }
                }
                for (auto *I : InstToErase) {
                    I->eraseFromParent();
                }
            }

            for (auto &Info : FloatStack) {
                auto *F = Info.F;
                if (SkipGvec and F->getName().contains("gvec")) {
                    continue;
                }
                OF << *F << "\n";
            }
        }

        return llvm::PreservedAnalyses::none();
    }
};

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo()
{
    return {.APIVersion = LLVM_PLUGIN_API_VERSION,
            .PluginName = "DumpFP",
            .PluginVersion = "v0.1",
            .RegisterPassBuilderCallbacks = [](llvm::PassBuilder& PB) {
                // for usage with opt
                PB.registerPipelineParsingCallback(
                    [](auto Name, llvm::ModulePassManager& PM,
                       auto /* PipelineElement*/) {
                        if (Name == "dump-fp") {
                            // if opt command line argument matches "my-pass"
                            // add the pass
                            PM.addPass(DumpFP{});
                            return true;
                        }
                        return false;
                    });
                // for usage with clang
                PB.registerOptimizerLastEPCallback(
                    [](llvm::ModulePassManager& PM, llvm::OptimizationLevel /* Level */) {
                        PM.addPass(DumpFP{});
                    });
            }};
}

