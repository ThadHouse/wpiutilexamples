#include "llvm/StringRef.h"
#include "llvm/Twine.h"
#include "llvm/SmallString.h"
#include "llvm/SmallVector.h"
#include "llvm/raw_ostream.h"

#include <iostream>

// Passing a simple string to a function to print it.
// Just pass a string ref. Note always pass StringRefs by value
// No reference
void SimpleStringPass(llvm::StringRef str) {
  // Use string directly in anything that takes a stringref or a std::string
  // such as llvm::outs or std::cout

  // llvm::outs is the most efficent way to print things
  // llvm::errs is the error stream, not usually necessary
  llvm::outs() << str << "\n";

  // Cout works too
  std::cout << str << std::endl;

  // Implicit conversion to std::string
  std::string s_str = str;

  // Can also explicitly convert with the .str() function
  std::string ss_str = str.str();


  // In some cases, a null termincated C string might be required
  // However, StringRefs are not required to be null termincated.
  // Therefore, the c_str function requires a passed in buffer
  llvm::SmallVector<char, 128> buf;
  const char* c_str = str.c_str(buf);

  // The above is heavily prefered over the following code
  // However, the following is still valid.
  std::string cs_str = str;
  const char* cc_str = cs_str.c_str();

  // However, the following is not valid. The std::string from the str()
  // call is a temperary, so its memory because invalid as soon as the
  // next line happens.
  const char* ccc_str = str.str().c_str();
}


void CallingSimpleString() {
  // A StringRef function can be called multiple ways

  // A standard quoted string works
  SimpleStringPass("Hello Raw CStr");

  // A std::string works
  std::string s_str = "Hello World from a std::string";
  SimpleStringPass(s_str);

  // A C string works
  const char* r_str = "Hello again raw";
  SimpleStringPass(r_str);

  // A stringref works
  llvm::StringRef val{"Hello from a stringRef" + 5};
  SimpleStringPass(val);

  // A StringRef can be created from a raw pointer and a length
  char data[] = {'h', 'e', 'l', 'l', 'o'};
  llvm::StringRef arr{data, 5};
  // Which can then be passed
  SimpleStringPass(arr);

  // The above can be simplified inline as well
  SimpleStringPass(llvm::StringRef{data, 5});

}


// The above work great for single strings, but a call like the following would be
// inneficient as it would implicitly create multiple std::strings
// To solve this, the Twine API can be used
// Pass as a const llvm::Twine&
void EndOfTwine(const llvm::Twine& twine) {
  // From here, I can:

  // Get a std::string of the entire twine
  std::string s_str = twine.str();

  // Get a StringRef to the twine
  // Note requires a buffer
  llvm::SmallVector<char, 128> srBuf;
  llvm::StringRef sRef = twine.toStringRef(srBuf);

  // Get a guarenteed null termated StringRef to the twine
  // Also requires a buffer, and recommend using the .data() afterward
  llvm::SmallVector<char, 128> nullBuf;
  llvm::StringRef nsRef = twine.toNullTerminatedStringRef(nullBuf);
  const char* nsData = nsRef.data();
  // Note the above data command is only a c_str for this specific instance
  // It just avoids the need for a second buffer.

  // There is also a toVector command, but the API looks the same as
  // toStringRef, and it does the same thing other then it doesn't return
  // a stringref, so that is prefered.
}

// The following shows what can be done in the middle of a twine
void MiddleOfTwine(const llvm::Twine& twine) {
  // I can pass straight through
  EndOfTwine(twine);

  // I Can add a string.
  EndOfTwine(twine + "Hello");

  // I can add anything I can construct a twine from.
  // Use intellisense to figure out
  // But can be vectors, ints, strings, etc

  // I can even do 3 in a row
  // Note ints must be constructed, they are not implicit
  EndOfTwine(twine + "Hello" + llvm::Twine(5));
}


void StartOfTwine() {
  // I can start a twine with anything I can construct a twine from
  MiddleOfTwine("Hello");

  // Even can do concatinations at the starts
  MiddleOfTwine(llvm::Twine(42) + "Hello");
}
