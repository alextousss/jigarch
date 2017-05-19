#include <iostream>
#include <cstring>
#include <boost/filesystem.hpp>
#include <sstream>

#include "encryptedarchive.hpp"

namespace fs = boost::filesystem;
using namespace std;

#define VERSION 0.1


int main(int argc, char *argv[])

{

  if( !( argc > 2 ) )
  {
    cout << "jigarch v " << VERSION << endl;
    cout << "\nSimply manage archives with basic encryption.\n" << endl;
    cout << "To add a file/folder to an archive           : 'jigarch -crypt <path to archive> <path to file/folder>' " << endl;
    cout << "To decrypt a file/folder from an archive     : 'jigarch -decrypt <path to archive> <path to destination folder> <file/folder to decrypt>' \n" << endl;
    cout << "jigarch is still currently in developpement, use it at your own risks !" << endl;
    cout << "If you want a feature to be added, open an issue on github.\n" << endl;
  }
  else
  {
    EncryptedArchive archive(argv[2]);

    if( !strcmp( argv[1], "-crypt") )
    {
      cout << "Quelle est la passphrase souhaitée pour chiffrer les données ?\n";
      cout << "Entrez une passphrase : ";
      string password;
      getline (cin,password);

      archive.encrypt(argv[3], password);
    }
    else if( !strcmp( argv[1], "-decrypt") )
    {
      cout << "Quelle est la passphrase souhaitée pour chiffrer les données ?\n";
      cout << "Entrez une passphrase : ";
      string password;
      getline (cin,password);

      archive.decrypt(argv[3], argv[4], password);

    }
  }
}
