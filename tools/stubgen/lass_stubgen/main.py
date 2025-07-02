# *** BEGIN LICENSE INFORMATION ***
#
# The contents of this file are subject to the Common Public Attribution License
# Version 1.0 (the "License"); you may not use this file except in compliance with
# the License. You may obtain a copy of the License at
# https://lass.cocamware.com/cpal-license. The License is based on the
# Mozilla Public License Version 1.1 but Sections 14 and 15 have been added to cover
# use of software over a computer network and provide for limited attribution for
# the Original Developer. In addition, Exhibit A has been modified to be consistent
# with Exhibit B.
#
# Software distributed under the License is distributed on an "AS IS" basis, WITHOUT
# WARRANTY OF ANY KIND, either express or implied. See the License for the specific
# language governing rights and limitations under the License.
#
# The Original Code is LASS - Library of Assembled Shared Sources.
#
# The Initial Developer of the Original Code is Bram de Greve and Tom De Muer.
# The Original Developer is the Initial Developer.
#
# All portions of the code written by the Initial Developer are:
# Copyright (C) 2025 the Initial Developer.
# All Rights Reserved.
#
# Contributor(s):
#
# Alternatively, the contents of this file may be used under the terms of the
# GNU General Public License Version 2 or later (the GPL), in which case the
# provisions of GPL are applicable instead of those above.  If you wish to allow use
# of your version of this file only under the terms of the GPL and not to allow
# others to use your version of this file under the CPAL, indicate your decision by
# deleting the provisions above and replace them with the notice and other
# provisions required by the GPL License. If you do not delete the provisions above,
# a recipient may use your version of this file under either the CPAL or the GPL.
#
# *** END LICENSE INFORMATION ***

"""Generate *.pyi stubs for Lass Python bindings using libclang."""

import functools
import multiprocessing
import subprocess
import sys
from argparse import ArgumentParser
from pathlib import Path

from .generator import StubGenerator, StubGeneratorError
from .parser import ParseError, Parser
from .stubdata import DuplicateError, StrPath, StubData


def main(
    argv: list[str],
    *,
    parser_type: type[Parser] = Parser,
    generator_type: type[StubGenerator] = StubGenerator,
) -> int:
    parser = ArgumentParser(description=__doc__, fromfile_prefix_chars="@")
    parser.add_argument(
        "path", nargs="+", help="Source files with Lass Python bindings to parse"
    )
    parser.add_argument(
        "-I",
        "--include-directory",
        dest="include_dirs",
        metavar="<dir>",
        action="append",
        default=[],
        help="Additional include directories for libclang",
    )
    parser.add_argument(
        "-isystem",
        dest="system_include_dirs",
        metavar="<dir>",
        action="append",
        default=[],
        help="Additional system include directories for libclang",
    )
    parser.add_argument(
        "-D",
        "--define-macro",
        dest="defines",
        metavar="<define>",
        action="append",
        default=[],
        help="Preprocessor defines for libclang",
    )
    parser.add_argument(
        "--arg",
        dest="args",
        metavar="<arg>",
        action="append",
        default=[],
        help="Additional arguments for libclang",
    )
    parser.add_argument(
        "--output-dir",
        metavar="<output-dir>",
        type=str,
        help="Output directory for generated stubs",
    )
    parser.add_argument(
        "--package",
        metavar="<package>",
        type=str,
        help="""Package name that the module belongs to. It will be prepended to the
module name to form the fully qualified name of the module.""",
    )
    parser.add_argument(
        "--export",
        metavar="<json-file>",
        type=str,
        help="""Export stub data to JSON file. This can later be imported in another
lass_stubgen run using the --import option.""",
    )
    parser.add_argument(
        "--import",
        dest="imports",
        metavar="<json-file>",
        action="append",
        default=[],
        help="""Import previously exported stub data from JSON file. This imported data
will be used to resolve C++ names from modules currently being parsed. This is useful
for parsing modules that depend on other modules that have already been parsed. The
imported modules will be marked as imported and no *.pyi stub files will be generated
for them. The imported modules will be added to the current module's imports list.
This option can be used multiple times to import multiple JSON files.""",
    )
    parser.add_argument(
        "--depfile",
        dest="depfile",
        metavar="<depfile>",
        help="""Write file dependencies of stub data or PCH to <depfile>. Must be used
            with --export or with --save-pch.""",
    )
    parser.add_argument(
        "--parse-only",
        action="store_true",
        default=False,
        help="Only parse the source files and generate the stub data, don't write *.pyi files",
    )
    parser.add_argument(
        "--pch-path",
        dest="pch_path",
        metavar="<pch-file>",
        help="""Precompiled header file to include in the parsing. This is used to speed
            up parsing of large projects. The PCH file must be generated with the same
            compiler and flags as the source files being parsed. When combined with the
            --save-pch flag, the first source fill will not generate stub data but will
            be saved as PCH file. The other source files will be processed normally.""",
    )
    parser.add_argument(
        "--save-pch",
        action="store_true",
        default=False,
        help="""Parse the first source file as PCH file and save to <pch-file>. No
            stubdata will be generated for this first source file. The other source
            files will be parsed normally, using the PCH file, and will generate stub
            data normally. Must be used with --pch-path.""",
    )
    parser.add_argument(
        "--num-threads",
        metavar="<num-threads>",
        type=int,
        default=0,
        help="""Parse source files in paralel with maximum <num-threads> parallel jobs.
If not provided, the number of threads will be set to the number of CPU cores.""",
    )
    parser.add_argument(
        "--with-signatures",
        action="store_true",
        default=False,
        help="Add C++ signatures as comments to the generated stubs, for debugging",
    )
    parser.add_argument(
        "--quiet", action="store_true", default=False, help="Be less chatty"
    )
    parser.add_argument(
        "--debug-connect", type=int, default=0, help="Port to connect to for debugging"
    )

    args = parser.parse_args(argv)

    if args.debug_connect:
        import debugpy  # type: ignore # noqa: I001

        debugpy.connect(args.debug_connect)

    if args.depfile and not (args.export or args.save_pch):
        parser.error("--depfile can only be used with --export or --save-pch.")
    if args.save_pch and not args.pch_path:
        parser.error("--save-pch can only be used with --pch-path.")

    try:
        stubdata: StubData = parse(
            source_paths=args.path,
            include_dirs=args.include_dirs,
            system_include_dirs=args.system_include_dirs,
            defines=args.defines,
            args=args.args,
            package=args.package,
            imports=args.imports,
            export=args.export,
            depfile=args.depfile,
            pch_path=args.pch_path,
            save_pch=args.save_pch,
            num_threads=args.num_threads,
            quiet=args.quiet,
            parser_type=parser_type,
        )
    except ParseError as err:
        for error in err.errors:
            print(error, file=sys.stderr)
        for note in err.__notes__:
            print(note, file=sys.stderr)
        print(f"{len(err.errors)} parse errors found, aborting", file=sys.stderr)
        return 1
    except DuplicateError as err:
        print(f"Error: {str(err)}", file=sys.stderr)
        return 1

    if not args.parse_only:
        try:
            generate(
                stubdata,
                output_dir=args.output_dir,
                with_signature=args.with_signatures,
                quiet=args.quiet,
                generator_type=generator_type,
            )
        except StubGeneratorError as err:
            print(f"Error: {str(err)}", file=sys.stderr)
            for note in err.__notes__:
                print(note, file=sys.stderr)
            return 1
    return 0


def parse(
    source_paths: list[StrPath],
    *,
    include_dirs: list[StrPath] | None = None,
    system_include_dirs: list[StrPath] | None = None,
    defines: list[str] | None = None,
    args: list[str] | None = None,
    package: str | None = None,
    imports: list[StrPath] | None = None,
    export: StrPath | None = None,
    depfile: StrPath | None = None,
    pch_path: StrPath | None = None,
    save_pch: bool = False,
    num_threads: int = 0,
    quiet: bool = False,
    parser_type: type[Parser] = Parser,
) -> StubData:
    """
    Parse the given source files and return a StubData object.
    """
    source_paths_ = [Path(path) for path in source_paths]
    imports_ = [Path(path) for path in imports or []]

    assert not (depfile and not (export or save_pch)), (
        "depfile can only be used with export or save_pch"
    )
    assert not (save_pch and not pch_path)

    parser = functools.partial(
        _parse_file,
        include_dirs=include_dirs,
        system_include_dirs=system_include_dirs,
        defines=defines,
        args=args,
        package=package,
        pch_path=pch_path,
        quiet=quiet,
        parser_type=parser_type,
    )

    if save_pch:
        # Assume the first source file is the precompiled header
        pch_header = source_paths_.pop(0)
        assert pch_path, "save_pch can only be used with pch_path"
        stubdata_ = parser(pch_header, save_pch=True)
        if depfile:
            stubdata_.write_depfile(depfile, target=pch_path)

    stubdata = StubData(package=package)
    for import_ in imports_:
        import_data = StubData.load(import_, imported=True)
        stubdata.update(import_data)

    if num_threads == 1:
        for path in source_paths_:
            stubdata.update(parser(path))
    else:
        if not num_threads:
            num_threads = multiprocessing.cpu_count()
        num_threads = min(num_threads, len(source_paths_)) or 1
        with multiprocessing.Pool(num_threads) as pool:
            for stubdata_ in pool.imap_unordered(parser, source_paths_):
                stubdata.update(stubdata_)

    stubdata.fix_fully_qualified_names()

    if export:
        stubdata.dump(export, depfile=depfile)

    return stubdata


def _parse_file(
    source_path: Path,
    *,
    package: str | None,
    include_dirs: list[StrPath] | None,
    system_include_dirs: list[StrPath] | None,
    defines: list[str] | None,
    args: list[str] | None,
    pch_path: StrPath | None = None,
    save_pch: bool = False,
    quiet: bool = False,
    parser_type: type[Parser],
) -> StubData:
    try:
        if source_path.suffix == ".json":
            return StubData.load(source_path)  # this is a stubdata file, let's load it

        if not quiet:
            print(f"Parsing {source_path}...", file=sys.stderr)
        parser = parser_type(
            package=package,
            include_dirs=include_dirs,
            system_include_dirs=system_include_dirs,
            defines=defines,
            args=args,
            pch_path=pch_path,
        )
        parser.parse(source_path, save_pch=save_pch)
        stubdata = parser.stubdata

        return stubdata
    except Exception as err:
        err.add_note(f"While parsing file {source_path}")
        raise err


def _name(path: StrPath) -> str:
    """Return name of file without any extension, so before first dot"""
    return Path(path).name.split(".")[0]


def generate(
    stubdata: StubData,
    *,
    output_dir: StrPath | None = None,
    with_signature: bool = False,
    quiet: bool = False,
    generator_type: type[StubGenerator] = StubGenerator,
) -> None:
    output_dir_ = Path(output_dir) if output_dir else None
    generator = generator_type(stubdata)
    for mod_def in stubdata.modules.values():
        if mod_def.imported:
            continue
        if output_dir_:
            assert mod_def.fully_qualified_name
            rel_path = mod_def.fully_qualified_name.replace(".", "/")
            output_file = output_dir_ / f"{rel_path}.pyi"
            output_file.parent.mkdir(parents=True, exist_ok=True)
            if not quiet:
                print(f"Writing {output_file}...", file=sys.stderr)
            try:
                with open(output_file, "w", encoding="utf-8") as file:
                    generator.write_module(
                        mod_def, file=file, with_signature=with_signature
                    )
                reformat(output_file)
            except Exception as err:
                err.add_note(f"While generating stubs {output_file}")
                raise
        else:
            print(f"# ============= {mod_def.py_name} =============", file=sys.stderr)
            generator.write_module(mod_def, file=sys.stdout)


def reformat(file: Path) -> None:
    """
    Reformat the file using ruff
    """
    bin_dir = Path(sys.executable).parent
    ruff = bin_dir / "ruff"
    try:
        subprocess.run(
            [ruff, "check", "--fix-only", file],
            check=True,
            capture_output=True,
            text=True,
        )
        subprocess.run(
            [ruff, "format", file], check=True, capture_output=True, text=True
        )
    except subprocess.CalledProcessError as err:
        raise StubGeneratorError(err.stderr or str(err)) from None
