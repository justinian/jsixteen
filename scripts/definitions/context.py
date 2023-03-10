class NotFound(Exception): pass

class Context:
    def __init__(self, path, verbose=False):
        if isinstance(path, str):
            self.__paths = [path]
        else:
            self.__paths = path

        self.__closed = set()
        self.__verbose = verbose

        self.__deps = {}

        self.objects = dict()
        self.interfaces = dict()

    verbose = property(lambda self: self.__verbose)

    def find(self, filename):
        from os.path import exists, isabs, join

        if exists(filename) or isabs(filename):
            return filename

        for path in self.__paths:
            full = join(path, filename)
            if exists(full):
                return full

        raise NotFound(filename)

    def parse(self, filename):
        pending = set()
        pending.add(filename)

        from .parser import LarkError
        from .parser import Lark_StandAlone as Parser
        from .transformer import DefTransformer

        objects = {}
        interfaces = {}

        while pending:
            name = pending.pop()
            self.__closed.add(name)

            path = self.find(name)

            parser = Parser(transformer=DefTransformer(name))

            try:
                imps, objs, ints = parser.parse(open(path, "r").read())
            except LarkError as e:
                import sys
                import textwrap
                print(f"\nError parsing {name}:", file=sys.stderr)
                print(textwrap.indent(str(e), "    "), file=sys.stderr)
                sys.exit(1)

            objects.update(objs)
            interfaces.update(ints)

            self.__deps[name] = imps

            pending.update(imps.difference(self.__closed))

        from .types import ObjectRef
        ObjectRef.connect(objects)

        for obj in objects.values():
            for method in obj.methods:
                caps = method.options.get("cap", list())
                for cap in caps:
                    if not cap in obj.caps:
                        from .errors import UnknownCapError
                        raise UnknownCapError(f"Unknown capability {cap} on {obj.name}::{method.name}")

        self.objects.update(objects)
        self.interfaces.update(interfaces)

    def deps(self):
        return {self.find(k): tuple(map(self.find, v)) for k, v in self.__deps.items()}
