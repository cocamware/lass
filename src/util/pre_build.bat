@echo off

echo Code generation ...

..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl object_factory.tmpl.h object_factory.h 10
..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl clone_factory.tmpl.h clone_factory.h 10

..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl callback_x.tmpl.h callback_$x.h 10
..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl callback_r_x.tmpl.h callback_r_$x.h 10
..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl impl\dispatcher_x.tmpl.h impl\dispatcher_$x.h 10
..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl impl\dispatcher_r_x.tmpl.h impl\dispatcher_r_$x.h 10

..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl pyobject_macros.tmpl.h pyobject_macros.h 10
..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl pyobject_call.tmpl.inl pyobject_call.inl 10

..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl thread_fun.tmpl.h thread_fun.h 10
..\..\tools\perl-bin\perl5.8.1.exe -I..\..\tools\perl-bin\ ..\..\tools\param_expander.pl thread_fun.tmpl.inl thread_fun.inl 10
