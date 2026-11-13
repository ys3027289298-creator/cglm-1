s/^\s*\.(L|LFB|LFE|LBB|LBE)[A-Za-z0-9_]+//
s/\.(cfi_[A-Za-z_]+|size|p2align|text|globl|type|ident|section|note\.GNU-stack|addrsig|intel_syntax).*$//
/^\s*$/d
s/^[[:space:]]*//
