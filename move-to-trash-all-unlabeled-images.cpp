#include <dlib/dir_nav/dir_nav_extensions.h>
#include <dlib/unicode/unicode.h>
#include "cpp-move-file-to-trash/move-file-to-trash.h"

void move_to_trash_if_exists(const std::string& filename)
{
    if (dlib::file_exists(filename)) {
        const std::wstring wide_filename = dlib::convert_mbstring_to_wstring(filename);
        move_file_to_trash(wide_filename);
    }
};

int main(int argc, char** argv)
try {
    if (argc != 2)
    {
        std::cout << "Usage: " << std::endl;
        std::cout << "> move-to-trash-all-unlabeled-images /path/to/anno/data" << std::endl;
        return 1;
    }

    const std::string mask_image_suffix = "_mask.png";

    std::cout << "Searching for images..." << std::endl;

    const std::vector<dlib::file> image_files = dlib::get_files_in_directory_tree(argv[1],
        [](const dlib::file& name) {
            if (dlib::match_ending("_mask.png")(name)) {
                return false;
            }
            if (dlib::match_ending("_result.png")(name)) {
                return false;
            }
            return dlib::match_ending(".jpeg")(name)
                || dlib::match_ending(".jpg")(name)
                || dlib::match_ending(".png")(name);
        });

    std::cout << "Found " << image_files.size() << " images..." << std::endl;

    std::vector<dlib::file> image_files_with_no_labels;

    for (const auto& file : image_files) {

        const std::string& full_name = file.full_name();

        const std::string label_filename = full_name + "_mask.png";

        if (!dlib::file_exists(label_filename)) {
            image_files_with_no_labels.push_back(full_name);
        }
    }

    std::cout << "Out of these, " << image_files_with_no_labels.size() << " have no labels." << std::endl;
    std::cout << "These image files will be moved to trash. Proceed ? [y/N] ";

    int proceed = std::cin.get();

    if (proceed == 'y' || proceed == 'Y') {
        size_t i = 0;
        for (const auto& file : image_files_with_no_labels) {
            std::cout << "\rMoving to trash file " << (++i) << " / " << image_files_with_no_labels.size() << "...";
            move_to_trash_if_exists(file.full_name());
            move_to_trash_if_exists(file.full_name() + "_result.png");
            move_to_trash_if_exists(file.full_name() + "_result.json");
        }
        std::cout << std::endl << "Done!" << std::endl;
    }
    else {
        std::cout << "Fair enough." << std::endl;
    }    
}
catch (std::exception& e) {
    std::cerr << std::endl << e.what() << std::endl;
}