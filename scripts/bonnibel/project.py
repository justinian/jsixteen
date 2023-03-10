from . import BonnibelError

class Project:
    def __init__(self, root):
        from .version import git_version

        self.root = root
        self.version = git_version(root)

    def __str__(self):
        return f"{self.name} {self.version.major}.{self.version.minor}.{self.version.patch}-{self.version.sha}"

    def generate(self, root, output, modules, config, manifest_file):
        import sys
        import bonnibel
        from os.path import join
        from ninja.ninja_syntax import Writer
        from .target import Target

        targets = set()
        for mod in modules.values():
            targets.update({Target.load(root, t, config) for t in mod.targets})

        with open(output / "build.ninja", "w") as buildfile:
            build = Writer(buildfile)

            build.comment("This file is automatically generated by bonnibel")
            build.variable("ninja_required_version", "1.3")
            build.variable("build_root", output)
            build.variable("source_root", root)
            build.newline()

            build.include(root / "configs" / "rules.ninja")
            build.newline()

            build.variable("version_major", self.version.major)
            build.variable("version_minor", self.version.minor)
            build.variable("version_patch", self.version.patch)
            build.variable("version_sha", self.version.sha)
            build.newline()

            build.variable("cogflags", [
                "-I", "${source_root}/scripts",
                "-D", "definitions_path=${source_root}/definitions",
                ])
            build.newline()

            for target in targets:
                build.subninja(output / target.name / "target.ninja")
            build.newline()

            for mod in modules.values():
                build.subninja(output / f"headers.{mod.name}.ninja")
            build.newline()

            build.build(
                rule = "touch",
                outputs = "${build_root}/.all_headers",
                implicit = [f"${{build_root}}/include/{m.name}/.headers.phony"
                    for m in modules.values() if m.public_headers],
                )
            build.build(
                rule = "phony",
                outputs = ["all-headers"],
                inputs = ["${build_root}/.all_headers"])

            debugroot = output / ".debug"
            debugroot.mkdir(exist_ok=True)

            fatroot = output / "fatroot"
            fatroot.mkdir(exist_ok=True)

            fatroot_content = []

            def add_fatroot(source, entry):
                output = join(entry.location, entry.output)
                fatroot_output = f"${{build_root}}/fatroot/{output}"

                build.build(
                    rule = "cp",
                    outputs = [fatroot_output],
                    inputs = [source],
                    variables = {
                        "name": f"Installing {output}",
                    })

                fatroot_content.append(fatroot_output)
                build.newline()

            def add_fatroot_exe(entry):
                input_path = f"${{build_root}}/{entry.target}/{entry.output}"
                intermediary = f"${{build_root}}/{entry.output}"

                build.build(
                    rule = "strip",
                    outputs = [intermediary],
                    inputs = [input_path],
                    implicit = [f"{input_path}.dump"],
                    variables = {
                        "name": f"Stripping {entry.module}",
                        "debug": f"${{build_root}}/.debug/{entry.output}.debug",
                    })

                add_fatroot(intermediary, entry)

            from .manifest import Manifest
            manifest = Manifest(manifest_file, modules)

            add_fatroot_exe(manifest.kernel)
            add_fatroot_exe(manifest.init)
            for program in manifest.programs:
                add_fatroot_exe(program)

            syms = manifest.add_data("symbol_table.dat",
                    manifest.kernel.location, "Symbol table", ("symbols",))

            sym_out = f"${{build_root}}/symbol_table.dat"
            build.build(
                rule = "makest",
                outputs = [sym_out],
                inputs = [f"${{build_root}}/{modules['kernel'].output}"],
                )
            add_fatroot(sym_out, syms)

            bootloader = "${build_root}/fatroot/efi/boot/bootx64.efi"
            build.build(
                rule = "cp",
                outputs = [bootloader],
                inputs = ["${build_root}/boot/boot.efi"],
                variables = {
                    "name": "Installing bootloader",
                })
            build.newline()

            boot_config = join(fatroot, "jsix_boot.dat")
            manifest.write_boot_config(boot_config)

            build.build(
                rule = "makefat",
                outputs = ["${build_root}/jsix.img"],
                inputs = ["${source_root}/assets/diskbase.img"],
                implicit = fatroot_content + [bootloader],
                variables = {"name": "jsix.img"},
                )
            build.newline()

            default_assets = {
                "UEFI Variables": ("ovmf/x64/ovmf_vars.fd", "ovmf_vars.fd"),
                "GDB Debug Helpers": ("debugging/jsix.elf-gdb.py", "jsix.elf-gdb.py"),
            }

            for name, assets in default_assets.items():
                p = root / "assets" / assets[0]
                out = f"${{build_root}}/{assets[1]}"
                build.build(
                    rule = "cp",
                    outputs = [out],
                    inputs = [str(p)],
                    variables = {"name": name},
                    )
                build.default([out])
                build.newline()

            compdb = "${source_root}/compile_commands.json"

            build.rule("regen",
                command = " ".join([str(root / 'configure')] + sys.argv[1:]),
                description = "Regenerate build files",
                generator = True,
                )
            build.newline()

            regen_implicits = \
                [f"{self.root}/configure", str(manifest_file)] + \
                [str(mod.modfile) for mod in modules.values()]

            for target in targets:
                regen_implicits += target.depfiles

            build.build(
                rule = "compdb",
                outputs = [compdb],
                implicit = regen_implicits,
                )
            build.default([compdb])
            build.newline()

            build.build(
                rule = "regen",
                outputs = ['build.ninja'],
                implicit = regen_implicits,
                implicit_outputs = 
                    [f"module.{mod.name}.ninja" for mod in modules.values()] +
                    [f"{target.name}/target.ninja" for target in targets] +
                    [boot_config],
                )

            build.newline()
            build.default(["${build_root}/jsix.img"])

        for target in targets:
            mods = [m.name for m in modules.values() if target.name in m.targets]

            targetdir = output / target.name
            targetdir.mkdir(exist_ok=True)

            buildfilename = str(targetdir / "target.ninja")
            with open(buildfilename, "w") as buildfile:
                build = Writer(buildfile)
                build.comment("This file is automatically generated by bonnibel")
                build.newline()

                build.variable("target", target.name)
                build.variable("target_dir", output / target.name)
                build.newline()

                for name, value in target.items():
                    build.variable(name, value)

                build.newline()
                for kind in ('defs', 'run'):
                    for lang in ('c', 'cpp'):
                        deffile = str(output / target.name / f"{lang}.{kind}")

                        build.build(
                            rule = f"dump_{lang}_{kind}",
                            outputs = [deffile],
                            implicit = [buildfilename],
                        )
                        build.default(deffile)
                        build.newline()

                for mod in mods:
                    build.subninja(f"module.{mod}.ninja")
