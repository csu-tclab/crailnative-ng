#include "pocket_dispatcher.h"

#include <iostream>

#include "crail_directory.h"
#include "crail_file.h"
#include "crail_outputstream.h"

using namespace std;

PocketDispatcher::PocketDispatcher() {}

PocketDispatcher::~PocketDispatcher() {}

int PocketDispatcher::Initialize(string address, int port) {
  return this->crail_.Initialize(address, port);
}

int PocketDispatcher::MakeDir(string name) {
  unique_ptr<CrailNode> crail_node =
      crail_.Create(name, FileType::Directory, 0, 0, true);
  if (!crail_node) {
    cout << "makedir failed " << endl;
    return -1;
  }
  return 0;
}

int PocketDispatcher::Lookup(string name) {
  unique_ptr<CrailNode> crail_node = crail_.Lookup(name);
  if (!crail_node) {
    cout << "lookup node failed" << endl;
    return -1;
  }
  return 0;
}

int PocketDispatcher::Enumerate(string name) {
  unique_ptr<CrailNode> crail_node = crail_.Lookup(name);
  if (!crail_node) {
    cout << "lookup node failed" << endl;
    return -1;
  }
  if (crail_node->type() != static_cast<int>(FileType::Directory)) {
    cout << "node is not a directory" << endl;
    return -1;
  }

  CrailNode *node = crail_node.get();
  CrailDirectory *directory = static_cast<CrailDirectory *>(node);
  directory->Enumerate();
  return 0;
}

int PocketDispatcher::PutFile(string local_file, string dst_file,
                              bool enumerable) {
  FILE *fp = fopen(local_file.c_str(), "r");
  if (!fp) {
    cout << "could not open local file " << local_file.c_str() << endl;
    return -1;
  }

  unique_ptr<CrailNode> crail_node =
      crail_.Create(dst_file, FileType::File, 0, 0, enumerable);
  if (!crail_node) {
    cout << "create node failed" << endl;
    return -1;
  }
  if (crail_node->type() != static_cast<int>(FileType::File)) {
    cout << "node is not a file" << endl;
    return -1;
  }

  CrailNode *node = crail_node.get();
  CrailFile *file = static_cast<CrailFile *>(node);
  unique_ptr<CrailOutputstream> outputstream = file->outputstream();

  shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(kBufferSize);
  while (size_t len = fread(buf->get_bytes(), 1, buf->remaining(), fp)) {
    buf->set_position(buf->position() + len);
    if (buf->remaining() > 0) {
      continue;
    }

    buf->Flip();
    while (buf->remaining() > 0) {
      if (outputstream->Write(buf) < 0) {
        return -1;
      }
    }
    buf->Clear();
  }

  if (buf->position() > 0) {
    buf->Flip();
    while (buf->remaining() > 0) {
      if (outputstream->Write(buf) < 0) {
        return -1;
      }
    }
  }

  fclose(fp);
  outputstream->Close();

  return 0;
}

int PocketDispatcher::GetFile(string src_file, string local_file) {
  unique_ptr<CrailNode> crail_node = crail_.Lookup(src_file);
  if (!crail_node) {
    cout << "lookup node failed" << endl;
    return -1;
  }
  if (crail_node->type() != static_cast<int>(FileType::File)) {
    cout << "node is not a file" << endl;
    return -1;
  }

  FILE *fp = fopen(local_file.c_str(), "w");
  if (!fp) {
    cout << "could not open local file " << local_file.c_str() << endl;
    return -1;
  }

  CrailNode *node = crail_node.get();
  CrailFile *file = static_cast<CrailFile *>(node);
  unique_ptr<CrailInputstream> inputstream = file->inputstream();

  shared_ptr<ByteBuffer> buf = make_shared<ByteBuffer>(kBufferSize);
  while (inputstream->Read(buf) > 0) {
    buf->Flip();
    while (buf->remaining()) {
      if (size_t len = fwrite(buf->get_bytes(), 1, buf->remaining(), fp)) {
        buf->set_position(buf->position() + len);
      } else {
        break;
      }
    }
    buf->Clear();
  }
  fclose(fp);
  inputstream->Close();

  return 0;
}

int PocketDispatcher::DeleteDir(string directory) {
  return crail_.Remove(directory, true);
}

int PocketDispatcher::DeleteFile(string file) {
  return crail_.Remove(file, false);
}

int PocketDispatcher::CountFiles(string directory) {
  int op = 5;
  return crail_.Ioctl((unsigned char)op, directory);
}
