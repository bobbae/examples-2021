Another compilation error

```
$ v run lisp.v
==================
/tmp/v/lisp.6924900048525050803.tmp.c:9277: error: invalid operand types for binary operation
/tmp/v/lisp.6924900048525050803.tmp.c:9277: error: invalid aggregate type for register load
...
==================
(Use `v -cg` to print the entire error message)

builder error: 
==================
C error. This should never happen.

If you were not working with C interop, this is a compiler bug, please report the bug using `v bug file.v`.

https://github.com/vlang/v/issues/new/choose

You can also use #help on Discord: https://discord.gg/vlang
```
