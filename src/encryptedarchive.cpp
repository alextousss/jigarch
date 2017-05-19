#include "encryptedarchive.hpp"


namespace fs = boost::filesystem;
using namespace std;


EncryptedArchive::EncryptedArchive() : path_to_archive("NULL")
{}

EncryptedArchive::EncryptedArchive(std::string path) : path_to_archive(path)
{}

void EncryptedArchive::encrypt(string path, string password)
{
  fs::path entry( path );

  if(!fs::exists( entry ))
  {
    cout << "File  " << entry << "  doesnt exist" << endl;
  }

  //fs::ifstream file_to_encrypt(path);
  fs::ofstream encrypted_archive(path_to_archive, ios::app);

  if( fs::is_directory(entry) )
  {
    encrypted_archive << "FOLDER:" << endl;
    encrypted_archive << entry.string() << endl;

    vector<fs::path> directory_content = getDirectoryContent(entry);

    for(unsigned int i = 0 ; i < directory_content.size() ; i++)
    {
      if( fs::is_directory( directory_content[i] ) )
      {
        encrypted_archive << "FOLDER:" << endl;
        encrypted_archive << fs::relative( directory_content[i], entry / ".." ).string() << endl;
      }
    }

    for(unsigned int i = 0 ; i < directory_content.size() ; i++)
    {
      if( fs::is_regular_file( directory_content[i] ) )
      {
        encrypted_archive << "FILE:" << endl;
        encrypted_archive << fs::relative( directory_content[i], entry / ".." ).string() << endl;
        encryptFile(encrypted_archive, directory_content[i].string(), password);
      }
    }

  }
  else if (fs::is_regular_file( entry ))
  {
    encrypted_archive << "FILE:" << endl;
    encrypted_archive << entry << endl;
    encryptFile(encrypted_archive, entry.string(), password);

  }

  encrypted_archive.close();

}


void EncryptedArchive::decrypt(string file, string path_out, string password)
{
  fs::path outpath(path_out);
  if(!fs::exists( path_to_archive ))
  {
    cout << "File  " << path_to_archive << "  doesnt exist" << endl;
  }

  fs::ifstream archive(path_to_archive);

  if( is_file(file) )
  {
    cout << "is file : " << file << endl;
    archive.seekg( getFilePosInArchive(file), ios::beg );
    decryptFile(archive, path_out, password);
  }
  else if( is_folder(file) )
  {
    cout << "is folder : " << file << endl;

    vector<fs::path> archive_structure = getArchiveStructure();
    vector<fs::path> archive_files = getArchiveFiles();

    vector<fs::path> folders_to_copy;
    vector<fs::path> files_to_decrypt;


    for(unsigned int i = 0 ; i < archive_structure.size() ; i++)
    {
      if(isPathInsideDirectory( file, archive_structure[i] ))
        folders_to_copy.push_back(archive_structure[i]);
    }


    for(unsigned int i = 0 ; i < archive_files.size() ; i++)
    {
      if(isPathInsideDirectory( file, archive_files[i] ))
        files_to_decrypt.push_back(archive_files[i]);
    }

    for(unsigned int i = 0 ; i < folders_to_copy.size() ; i++)
    {
      fs::create_directories(path_out / folders_to_copy[i]);
    }

    cout << "files:" << endl;
    for(unsigned int i = 0 ; i < files_to_decrypt.size() ; i++)
    {
      cout << files_to_decrypt[i].string() << endl;
      cout << getFilePosInArchive(files_to_decrypt[i].string()) << endl;
      archive.seekg( getFilePosInArchive(files_to_decrypt[i].string()), ios::beg );
      decryptFile(archive, (outpath / files_to_decrypt[i]).string(), password);
    }

  }

  archive.close();

}


vector<fs::path> EncryptedArchive::getArchiveStructure()
{
  vector<fs::path> archive_structure;

  string word;
  fs::ifstream archive(path_to_archive);

  while(getline( archive, word ))
  {
    if(word == "FOLDER:")
    {
      getline( archive, word );
      archive_structure.push_back(word);
      cout << word << endl;
    }
  }

  archive.close();

  return archive_structure;

}


vector<fs::path> EncryptedArchive::getArchiveFiles()
{
  vector<fs::path> archive_structure;

  string word;
  fs::ifstream archive(path_to_archive);

  while(getline( archive, word ))
  {
    if(word == "FILE:")
    {
      getline( archive, word );
      archive_structure.push_back(word);
    }
  }

  archive.close();

  return archive_structure;
}


int EncryptedArchive::getFilePosInArchive(std::string file)
{
  string word;
  fs::ifstream archive(path_to_archive);

  while(getline( archive, word))
  {
    if(word == "FILE:")
    {
      getline( archive, word) ;
      if(word == file)
      {
        return archive.tellg();
      }
    }
  }

  archive.close();

  return FILE_NOT_FOUND;
}

bool EncryptedArchive::isPathInsideDirectory(boost::filesystem::path directory, boost::filesystem::path path)
{
  fs::path path_to_decompose = path;

  while(path_to_decompose.has_parent_path() )
  {
    if( path_to_decompose.parent_path() != directory )
    {
      path_to_decompose = path_to_decompose.parent_path();
    }
    else
    {
      return true;
    }
  }

  return false;
}


vector<fs::path> EncryptedArchive::getDirectoryContent(boost::filesystem::path path)
{
  vector<fs::path> directory_content;

  fs::recursive_directory_iterator end_itr;

  for (fs::recursive_directory_iterator itr(path); itr != end_itr; ++itr)
  {
    if (is_regular_file(itr->path()) || is_directory(itr->path()) )
    {
      directory_content.push_back(itr->path());
    }
  }

  return directory_content;
}

void EncryptedArchive::encryptFile(fs::ofstream & outfile, string infile_loc, std::string password)
{
  fs::fstream infile;

  infile.open (infile_loc);

  char c = 0;
  unsigned int passwordIndex = 0;
  unsigned int bytes_to_encrypt = 0;

  infile.seekg(0, ios::end);  //On se déplace à la fin du fichier
  bytes_to_encrypt =  infile.tellg();
  bytes_to_encrypt--;

  infile.seekg(0, ios::beg);  //On se déplace au debut du fichier pour commencer a lire

  outfile << bytes_to_encrypt << endl; //On écrit cette valeur pour connaître la longueur a déchiffrer quand on déchiffrera

  infile.seekg(0, ios::beg); //On revient au début du fichier pour commencer le chiffrement

  while ( infile.get( c ) )
  {
    c = c + password[passwordIndex]; //Modification des caractères
    c = c - password[0];

    outfile << c;

    passwordIndex++; //Incrementation du password
    passwordIndex %= password.length(); //Remise à zéro quand on le passwordIndex est supérieur a la taille du password

  }

  outfile << endl;

  infile.close();
}

void EncryptedArchive::decryptFile(fs::ifstream & infile, string outfile_loc, string password)
{
  cout << outfile_loc << endl;
  fs::ofstream outfile;
  string word;

  unsigned int file_length = 0;
  getline(infile, word);
  stringstream myStream(word);
  myStream >> file_length;

	outfile.open (outfile_loc);

  char c;
  unsigned int passwordIndex = 0;

  for(unsigned int i = 0 ; i <= file_length ; i++) //On déchiffre en suivant la taille du bloc
	{
    infile.get( c );
	  c = c - password[passwordIndex]; //Modification des caractères
		c = c + password[0];
		outfile << c;
    passwordIndex++; //Incrementation du password
    passwordIndex %= password.length(); //Remise à zéro quand on le passwordIndex est supérieur a la taille du password

	}
  infile.ignore();
	outfile.close();

}

int EncryptedArchive::is_folder(boost::filesystem::path path)
{
  string word;
  fs::ifstream archive(path_to_archive);

  while(getline( archive, word))
  {
    if(word == "FOLDER:")
    {
      getline( archive, word) ;
      if(word == path.string())
      {
        return true;
      }
    }
    else if(word == "FILE:")
    {
      getline( archive, word) ;
      if(word == path.string())
      {
        return false;
      }
    }
  }

  archive.close();

  return FILE_NOT_FOUND;
}


int EncryptedArchive::is_file(boost::filesystem::path path)
{
  string word;
  fs::ifstream archive(path_to_archive);

  while(getline( archive, word))
  {
    if(word == "FILE:")
    {
      getline( archive, word) ;
      if(word == path.string())
      {
        return true;
      }
    }
    else if(word == "FOLDER:")
    {
      getline( archive, word) ;
      if(word == path.string())
      {
        return false;
      }
    }
  }

  archive.close();

  return FILE_NOT_FOUND;
}
