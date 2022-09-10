#include <sha_ila.h>
#include <ilang/vtarget-out/vtarget_gen.h>

/// the function to parse commandline arguments
VerilogVerificationTargetGenerator::vtg_config_t HandleArguments(int argc, char **argv);

/// To verify the IO ILA
void verifyTop(Ila& model, VerilogVerificationTargetGenerator::vtg_config_t vtg_cfg) {

  VerilogGeneratorBase::VlgGenConfig vlg_cfg;

  vtg_cfg.MemAbsReadAbstraction = true; // enable read abstraction
  vlg_cfg.pass_node_name = true;

  std::string RootPath = "..";
  std::string VerilogPath = RootPath + "/verilog/";
  std::string RefrelPath = RootPath + "/refinement/";
  std::string OutputPath = RootPath + "/verification/";

  VerilogVerificationTargetGenerator vg(
      {}, // no include
      {VerilogPath + "sha1_core.v",   VerilogPath + "reg2byte.v",
       VerilogPath + "reg16byte.v", VerilogPath + "reg32byte.v",
       VerilogPath + "reg256byte.v", VerilogPath + "sha_top.v",
       VerilogPath + "sha1_w_mem.v"},                // designs
      "sha_top",                                      // top_module_name
      RefrelPath + "ref-rel-var-map.json",            // variable mapping
      RefrelPath + "ref-rel-inst-cond.json",          // conditions of start/ready
      OutputPath,                                     // output path
      model.get(),                                    // model
      VerilogVerificationTargetGenerator::backend_selector::COSA, // backend: COSA
      vtg_cfg,  // target generator configuration
      vlg_cfg); // verilog generator configuration

  vg.GenerateTargets();
}

/// Build the model
int main(int argc, char **argv) {
  // extract the configurations
  auto vtg_cfg = HandleArguments(argc, argv);

  // build the SHA model
  SHA sha_ila_model;
  verifyTop(sha_ila_model.model, vtg_cfg);

  return 0;
}



VerilogVerificationTargetGenerator::vtg_config_t HandleArguments(int argc, char **argv) {
  // the solver, the cosa environment
  // you can use a commandline parser if desired, but since it is not the main focus of
  // this demo, we skip it

  // set ilang option, operators like '<' will refer to unsigned arithmetics
  SetUnsignedComparison(true); 
  
  VerilogVerificationTargetGenerator::vtg_config_t ret;

  for(unsigned p = 1; p<argc; p++) {
    std::string arg = argv[p];
    auto split = arg.find("=");
    auto argName = arg.substr(0,split);
    auto param   = arg.substr(split+1);

    if(argName == "Solver")
      ret.CosaSolver = param;
    else if(argName == "Env")
      ret.CosaPyEnvironment = param;
    else if(argName == "Cosa")
      ret.CosaPath = param;
    // else unknown
    else {
      std::cerr<<"Unknown argument:" << argName << std::endl;
      std::cerr<<"Expecting Solver/Env/Cosa=???" << std::endl;
    }
  }

  ret.CosaGenTraceVcd = true;

  return ret;
}
