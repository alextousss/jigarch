
#ifndef DEF_ENCRYPTEDARCHIVEMANAGER
#define DEF_ENCRYPTEDARCHIVEMANAGER

#include <iostream>
#include <boost/filesystem.hpp>
#include <cstring>
#include <sstream>

#define FILE_NOT_FOUND -1


class EncryptedArchive
{
public:
  EncryptedArchive();
  EncryptedArchive(std::string path);

  int open();
  int close();


  std::vector<boost::filesystem::path> getArchiveStructure();
  std::vector<boost::filesystem::path> getArchiveFiles();

  void encrypt(std::string path, std::string password);
  void decrypt(std::string file, std::string path_out, std::string password);

private:
  std::vector<boost::filesystem::path> getDirectoryContent(boost::filesystem::path path);

  void encryptFile(boost::filesystem::ofstream & outfile, std::string infile_loc, std::string password);
  void decryptFile(boost::filesystem::ifstream & infile, std::string outfile_loc, std::string password);

  int getFilePosInArchive(std::string file);

  bool isPathInsideDirectory(boost::filesystem::path directory, boost::filesystem::path path);

  int is_folder(boost::filesystem::path path);
  int is_file(boost::filesystem::path path);

  boost::filesystem::path path_to_archive;


};

#endif
