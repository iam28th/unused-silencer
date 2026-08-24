import lit.formats

import lit.llvm
from lit.llvm import llvm_config

# lit_config is a global instance of LitConfig (c)
lit.llvm.initialize(lit_config, config)

config.test_format = lit.formats.ShTest(not llvm_config.use_lit_shell)

config.name = 'Silencer'

config.suffixes = ['.cpp']

config.test_source_root = os.path.dirname(__file__)

config.substitutions.append((
    "%silencer_as_plugin",
    "clang -c -Xclang -verify "
    "-Xclang -load -Xclang %plugin_lib_dir/libSilencer.so "
    "-Xclang -plugin -Xclang silencer_plugin"
))
config.substitutions.append(('%plugin_lib_dir', config.plugin_lib_dir))

# means "search for these tools in this folder"
llvm_config.add_tool_substitutions(["clang", "clang++"], config.llvm_tools_dir)
llvm_config.add_tool_substitutions(["silencer"], config.plugin_tool_dir)
