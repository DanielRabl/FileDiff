#include <qpl/qpl.hpp>

constexpr bool is_character_visible(char c) {
	return qpl::u8_cast(c) >= 32;
}
bool check_diff(qpl::filesys::path path1, qpl::filesys::path path2) {

	std::string read1 = path1.read();
	std::string read2 = path2.read();

	bool equal = (read1 == read2);
	if (!equal) {
		if (read1.length() != read2.length()) {
			qpl::println("\ncontent differs: "); 
			qpl::println(path1, " size = ", qpl::memory_size_string(read1.length()));
			qpl::println(path2, " size = ", qpl::memory_size_string(read2.length()));
		}
		else {
			std::string string;
			bool sequence = false;

			bool visible = true;
			qpl::size start = 0u;
			qpl::size end = 0u;
			std::ostringstream stream;

			auto stop = qpl::min(read1.length(), read2.length());
			for (qpl::size i = 0u; i < stop; i++) {
				auto c1 = read1[i];
				auto c2 = read2[i];

				if (sequence || c1 != c2) {
					if (visible && (!is_character_visible(c1) || !is_character_visible(c2))) {
						visible = false;
					}

					if (!sequence) {
						start = i;
						visible = true;
					}
					sequence = true;

					if (c1 == c2) {

						qpl::size a = qpl::size_cast(qpl::max(qpl::isize{ 0 }, qpl::isize_cast(start) - 5));
						qpl::size b = qpl::size_cast(qpl::min(qpl::isize_cast(stop) - 1, qpl::isize_cast(i) + 5));

						auto str1 = read1.substr(a, b - a);
						auto str2 = read2.substr(a, b - a);
						sequence = false;

						stream << qpl::to_string("\nNOT EQUAL at ", start, '\n');
						if (visible) {
							stream << qpl::to_string("...", str1.substr(0, 600), "...\n");
							stream << qpl::to_string("...", str2.substr(0, 600), "...\n");
						}
						else {
							stream << qpl::to_string("...", qpl::hex_string(str1.substr(0, 300)), "...\n");
							stream << qpl::to_string("...", qpl::hex_string(str2.substr(0, 300)), "...\n");
						}
					}
				}
			}

			qpl::println("\ncontent differs: ");
			qpl::println(stream.str());
		}

		return false;
	}
	else {
		return true;
	}
}

void check(std::string path_str1, std::string path_str2) {

	qpl::filesys::path path1 = path_str1;
	qpl::filesys::path path2 = path_str2;

	bool result = true;
	if (path1.is_file() && path2.is_file()) {
		result = result && check_diff(path1, path2);
	}
	else if (path1.is_directory() && path2.is_directory()) {
		auto tree1 = path1.list_current_directory_tree();
		auto tree2 = path2.list_current_directory_tree();

		if (tree1.size() != tree2.size()) {
			qpl::println("\ndirectory sizes don't match: ");
			qpl::println(path1, " has ", tree1.size(), " contents");
			qpl::println(path2, " has ", tree1.size(), " contents");
			result = false;
		}
		else {
			for (qpl::size i = 0; i < tree1.size(); ++i) {
				auto p1 = tree1[i];
				auto p2 = tree2[i];

				if (i) {
					qpl::println();
					qpl::println_repeat("- ", 50);
					qpl::println();
				}

				qpl::println("\n#1 = ", p1, "\n#2 = ", p2);

				bool good = true;
				if (p1.is_file() && p2.is_file()) {
					auto r = check_diff(p1, p2);
					result = result && r;
					if (!r) {
						good = false;
					}
				}
				else if (p1.is_directory() && p2.is_directory()) {
					if (p1.get_full_name() != p2.get_full_name()) {
						qpl::println("\ndirectory names don't match: ");
						qpl::println(p1, " name = \"", p1.get_full_name(), "\"");
						qpl::println(p2, " name = \"", p2.get_full_name(), "\"");
						result = false;
						good = false;
					}
				}
				else {
					qpl::println("\ntypes don't match: ");
					qpl::println(p1, " is a ", p1.is_file() ? "file" : "directory");
					qpl::println(p2, " is a ", p2.is_file() ? "file" : "directory");
					result = false;
					good = false;
				}
				if (good) {
					qpl::println(">>> they matched.");
				}
				else {
					qpl::println("\n>>> they didn't match.");
				}
			}
		}
	}
	if (result) {
		qpl::println();
		qpl::println_repeat("- ", 50);
		qpl::println();
		qpl::println(">>> everything was equal.");
	}
	else {
		qpl::println();
		qpl::println_repeat("- ", 50);
		qpl::println();
		qpl::println(">>> there were differences.");
	}
	qpl::system_pause();
}

int main(int argc, char** argv) try {
	std::vector<std::string> args(argc - 1);
	for (int i = 1; i < argc; ++i) {
		args[i - 1] = argv[i];
	}
	if (args.size() != 2) {
		qpl::println("drag 2 files / directories onto this executable");
		qpl::system_pause();
	}
	check(args[0], args[1]);

	qpl::println("hello world!");
}
catch (std::exception& any) {
	qpl::println("caught exception:\n", any.what());
	qpl::system_pause();
}