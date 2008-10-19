import sys
import os
import optparse

script_dir = os.path.dirname(__file__)
base_dir = os.path.abspath(os.path.join(script_dir, os.pardir, os.pardir))
if not base_dir in sys.path:
	sys.path.insert(0, base_dir)

import tools.param_expander

EXCLUDES = [
	'test_common.cpp'
	]

def collect_unit_tests(dirname):
	test_sources = [fname for fname in os.listdir(dirname) if not fname in EXCLUDES and fname.startswith("test_") and fname.endswith(".cpp")]
	test_names = [os.path.splitext(x)[0] for x in test_sources]
	return test_names
	
def write_auto_test_suite(testdir, source, dest):
	test_names = collect_unit_tests(testdir)
	tools.param_expander.expandFileWithList(source, dest, test_names)

if __name__ == "__main__":
	parser = optparse.OptionParser()
	parser.add_option("-i", "--input", action="store", type="string", default="auto_suite_map.cpp.in", metavar="PATH", help="[default=%default]")
	parser.add_option("-o", "--output", action="store", type="string", default="auto_suite_map.cpp", metavar="PATH", help="[default=%default]")
	parser.add_option("-t", "--testdir", action="store", type="string", default=".", metavar="DIRNAME", help="[default=%default]")
	options, args = parser.parse_args()
	write_auto_test_suite(options.testdir, options.input, options.output)