@echo off

echo Code generation ...
..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl test_util_callback.tmpl.inl test_util_callback.inl 3
