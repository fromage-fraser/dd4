"""Exercise data preservation through both Scribe conversion directions."""
import importlib.util
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

TOOLS = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("are2src", TOOLS / "are2src.py")
are2src = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = are2src
SPEC.loader.exec_module(are2src)
SCRIBE = TOOLS / "scribe.pl"


class RoundTrips(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.directory.cleanup)
        self.folder = Path(self.directory.name)

    def rebuild(self, source):
        return are2src.compile_source(source, SCRIBE, self.folder)

    def test_editable_fields_and_weighted_specials(self):
        source = """$area
au Builder
ti Conversion example
ls 1
us 100
le 0
ue 100
bv 0
$mob
vn 99001
nm test creature
sh a test creature
lo A test creature waits here.
de
A creature for checking raw overrides.
~
lv 60
al 0
sx female
act 0
aff 0
bf 32
archetype kopru
rnk common
height 110
atk 17179869184
sp spec_cast_mage
sp2 spec_fascinating_gaze
sp3 none
spchance 70 30 0
"""
        original = self.rebuild(source)
        converted, retained = are2src.convert(original, SCRIBE)
        self.assertIn("$mob\n", converted)
        self.assertIn("bf 32\n", converted)
        self.assertIn("atk 17179869184\n", converted)
        self.assertIn("archetype kopru\n", converted)
        self.assertIn("SPECIALS", retained)
        self.assertEqual(are2src.read_sections(original), are2src.read_sections(self.rebuild(converted)))

    def test_native_legacy_combat_values(self):
        original = """#AREA Builder~ Legacy example~
1 100 0 100
#MOBILES
#99001
test~
a test~
A test is here.~
A test creature.~
0 0 0 S
10 2 3 4d5+6 7d8+9
0 12
8 8 1
#0
#$
"""
        source, retained = are2src.convert(original, SCRIBE)
        self.assertIn("MOBILES", retained)
        self.assertIn("4d5+6 7d8+9", source)
        self.assertEqual(are2src.read_sections(original)[1].data,
                         are2src.read_sections(self.rebuild(source))[1].data)

    def test_help_file_retains_every_record(self):
        original = "#HELPS\n0 EXAMPLE~\nA help entry.\n~\n0 $~\n#$\n"
        source, retained = are2src.convert(original, SCRIBE)
        self.assertEqual(retained, ["FILE (legacy data retained verbatim)"])
        self.assertEqual(self.rebuild(source), original)

    def test_unknown_section_is_retained_visibly(self):
        original = "#AREA Builder~ Future format~\n1 100 0 100\n#FUTURE\n123 preserve me\n#$\n"
        source, retained = are2src.convert(original, SCRIBE)
        self.assertIn("$native FILE", source)
        self.assertIn("123 preserve me", source)
        self.assertEqual(self.rebuild(source), original)

    def test_native_marker_collision(self):
        section = are2src.Section("HELPS", "0 EXAMPLE~\nEND_NATIVE_HELPS\n~\n0 $~\n", None)
        source = are2src.native(section)
        self.assertTrue(source.startswith("$native HELPS END_NATIVE_HELPS_\n"))

    def test_mixed_native_and_friendly_records_are_rejected(self):
        source = "$area\nau Builder\nti Mixed\nls 1\nus 100\nle 0\nue 100\nbv 0\n$recall\nrl 3001\n$native RECALL END\n3002\nEND\n"
        with self.assertRaises(are2src.AreaError):
            self.rebuild(source)

    def test_failed_import_does_not_replace_destination(self):
        source, destination = self.folder / "bad.are", self.folder / "saved.src"
        source.write_text("#AREA unfinished~\n", encoding="utf-8")
        destination.write_text("Keep my work.\n", encoding="utf-8")
        result = subprocess.run([sys.executable, str(TOOLS / "are2src.py"),
                                 str(source), str(destination), "--force"], capture_output=True)
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(destination.read_text(encoding="utf-8"), "Keep my work.\n")


if __name__ == "__main__":
    unittest.main()
