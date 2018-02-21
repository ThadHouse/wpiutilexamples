#include "support/json.h"
#include "support/raw_istream.h"
#include "llvm/raw_ostream.h"
#include "llvm/SmallVector.h"
#include "llvm/StringRef.h"


// C++ has no reflection support.
// Therefore, we have to manually serialize and deserialize our objects.
// To start, just declare a struct. Note this struct must be default constructable
// (Have either no constructor or one with no parameters)

struct Point {
    double x;
    double y;
};


// Then, we need to_json and from_json functions
// These must be in the same namespace as the struct
void to_json(wpi::json& j, const Point& p) {
    // Make sure J is empty
    j= wpi::json();
    // Place the individual objects like a dictionary
    j["x"] = p.x;
    j["y"] = p.y;
}

void from_json(const wpi::json& j, Point& p) {
    // Use ".at" instead of "[]" so exceptions will be thrown
    p.x = j.at("x").get<double>();
    p.y = j.at("y").get<double>();
}

struct Money {
    std::string currency;
    double value;
};

void from_json(const wpi::json& j, Money& m) {
    m.currency = j.at("currency").get<std::string>();
    m.value = j.at("value").get<double>();
}


void ReadJson() {
    // To start reading json, open the file.

    std::error_code code; // Need error code storage
    wpi::raw_fd_istream fileIn{"file.json", code};

    if (code.value() != 0) {
        llvm::errs() << "Error, could not load file\n";
        llvm::errs().flush();
        return;
    }

    // Parse either takes a file, or can take a string
    auto json = wpi::json::parse(fileIn);

    // Use array syntax to get individual objects
    auto pi = json["pi"];

    // Check its type
    if (pi.is_number()) {
        // If a simple type, can be directly converted
        double num = pi;
    }

    auto name = json["name"];
    if (name.is_string()) {
        // Do not store in a StringRef, store in a std::string
        std::string str = name;

        // The following syntax works if you want auto on the left
        auto str2 = name.get<std::string>();
    }

    // Getting anything out of a json returns another json object
    wpi::json obj = json["object"];
    // Because of this, we can either double index to find things
    std::string currency = json["object"]["currency"];
    // Or we can use our intermediate 'obj'
    std::string currency2 = obj["currency"];

    // Or because of our from_json function, we can directly convert
    // to our custom object
    Money m = obj;
    // or we don't need the intermediate json variable
    Money m2 = json["object"];


    // The deserialization works in a list as well
    llvm::SmallVector<Point, 16> list = json["objectlist"];

    auto size = list.size();

    for (auto&& i : list) {
        llvm::outs() << " " << i.x << " " << i.y << "\n";
    }
    llvm::outs().flush();

    // We could the reserialize at the end if we wanted.
    auto serialized = json.dump();
}

void WriteJson() {
    // Writing Json is easy.
    // First create a JSON
    wpi::json json;
    // Then treat it like a dictionary
    json["apple"] = "gross";
    // Can even nest
    json["grape"]["jelly"] = "yummy";

    wpi::json internal;
    internal["muahahah"] = 1234;
    internal["v2"] = true;

    json["intval"] = internal;


    // Then serialize. Use the parameter if you want indentation
    auto serialized = json.dump(4);

    // We can also searialize into a stream, for instance if we wanted to go straight to a file.

    {
        std::error_code code;

        // OpenFlags needs ReadWrite. This will cause it to not append
        llvm::raw_fd_ostream fileOut{"output.json", code, llvm::sys::fs::OpenFlags::F_RW};
        if (code.value() == 0) {
            // Can omit second parameter to not need pretty printing
            json.dump(fileOut, 4);
        }
    }

    // If we have an object we have a to_json for, we can assign that directory.
    // Also a container of objects that can be to_json'd works too
    // The warning below can be ignored.
    llvm::SmallVector<Point, 16> points{{1,2}, {3,34}, {8,62}};

    json = points;

    serialized = json.dump(2);

    llvm::outs() << serialized;
    llvm::outs().flush();
}
