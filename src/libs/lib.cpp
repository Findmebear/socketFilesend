#include "pack109.hpp"

#define KEY 42
#define X8  256
#define X16 65536
#define MASK4 0x000000FF
#define MASK8 0x00000000000000FF

vec slice(vec& bytes, int vbegin, int vend) {
  auto start = bytes.begin() + vbegin;
  auto end = bytes.begin() + vend + 1;
  vec result(vend - vbegin + 1);
  copy(start, end, result.begin());
  return result;
}


// ----------------------------------------
// PACK109_U8
// ----------------------------------------

// Serializing a u8 is simple: push the tag onto the vector, then the byte.
vec pack109::serialize(u8 item) {
  vec bytes;
  bytes.push_back(PACK109_U8);
  bytes.push_back(item);
  return bytes;
}

u8 pack109::deserialize_u8(vec bytes) {
  // To deserialize the u8, we need at least two bytes in the serialized
  // vector: one for the tag and one for the byte.
  if (bytes.size() < 2) {
    throw;
  }
  // Check for the correct tag
  if (bytes[0] == PACK109_U8) {
    // Directly return the byte. We can do this because the return type of the
    // function is the same as the contents of the vector: u8.
    return bytes[1];
  } else {
    // Throw if the tag is not a u8
    throw;
  }
}

// ----------------------------------------
// PACK109_U32
// ----------------------------------------

vec pack109::serialize(u32 item) {
  vec bytes;
  bytes.push_back(PACK109_U32);
  u32 mask = MASK4; // This mask will zero out the first 3 bytes
  // Look over the bytes of the integer. Integers have 4 bytes total.
  for(int i = 3; i >= 0; i--) {
    u32 to_shift = 8 * i; // Number of bytes to shift the item
    u32 shifted = item >> to_shift;
    // Mask the shifted bytes and cast as a char.
    // e.g. if the item is 0x12345678, and we shift 2 bytes:
    // shifted = 0x00001234
    // After the mask we have 0x00000034
    // After the cast we have 0x34. This value is pushed onto the vector.
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}

u32 pack109::deserialize_u32(vec bytes) {
  // The first byte is the tag, then we have 4 bytes for the payload, for a
  // total of 5 bytes
  if (bytes.size() < 5) {
    throw;
  }
  // To deserialize the u32, we need a container for it. 
  u32 deserialized_u32 = 0;
  if (bytes[0] == PACK109_U32) {
    int ix = 1;
    for(int i = 3; i >= 0; i--) {
      int to_shift = 8 * i;
      // We have to make sure to cast the byte to a u32 before shifting it.
      // If we don't, the bytes we are interested in will fall off:
      // e.g. if the byte is 0xAB and we want to left shift 2 bytes
      // bytes[ix] << to_shift = 0x00
      // (u32)bytes[ix] << to_shift = 0x00AB0000
      u32 shifted = (u32)bytes[ix] << to_shift;
      // Or is used here to put the shifted bytes into the container we
      // allocated outside of the for loop. e.g. if the byte vector is:
      // [AB,CD,EF,12]
      // Then deserialized_u32 will be:
      // 0xAB000000
      // 0xABCD0000
      // 0xABCDEF00
      // 0xABCDEF12
      // Over the 4 iterations of the loop
      deserialized_u32 = deserialized_u32 | shifted;
      ix++;
    }
    return deserialized_u32;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_U64
// ----------------------------------------

// Serializing a u64 works pretty much the same as serializing a u32. The big
// change is how many times we go through the for loop.
vec pack109::serialize(u64 item) {
  vec bytes;
  bytes.push_back(PACK109_U64);
  u64 mask = MASK8;
  for(int i = 7; i >= 0; i--) {
    int to_shift = 8 * i;
    u64 shifted = item >> to_shift;
    bytes.push_back((u8)(shifted & mask));
  }
  return bytes;
}

// Deserializing the u64 works the same as the u32. The big change here
// is the size of the container.
u64 pack109::deserialize_u64(vec bytes) {
  if (bytes.size() < 9) {
    throw;
  }
  u64 deserialized_u64 = 0; // This is a u64 now, so we can fit all the bytes
  if (bytes[0] == PACK109_U64) {
    int ix = 1;
    for(int i = 7; i >= 0; i--) {
      int to_shift = 8 * i;
      u64 shifted = (u64)bytes[ix] << to_shift;
      deserialized_u64 = deserialized_u64 | shifted;
      ix++;
    }
    return deserialized_u64;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_I8
// ----------------------------------------

vec pack109::serialize(i8 item) {
  vec bytes;
  bytes.push_back(PACK109_I8);
  // We can cast the i8 to a u8 and push because it's just a byte
  bytes.push_back((u8)item); 
  return bytes;
}

i8 pack109::deserialize_i8(vec bytes) {
  if (bytes.size() < 2) {
    throw;
  }
  if (bytes[0] == PACK109_I8) {
    // Get the byte and cast it to the correct return type
    return (i8)bytes[1];
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_I32
// ----------------------------------------

vec pack109::serialize(i32 item) {
  vec bytes;
  bytes.push_back(PACK109_I32);
  i32 mask = MASK4;
  for(int i = 3; i >= 0; i--) {
    int to_shift = 8 * i;
    i32 shifted = item >> to_shift;
    bytes.push_back((u8)(item & mask));
  }
  return bytes;
}

i32 pack109::deserialize_i32(vec bytes) {
  if (bytes.size() < 5) {
    throw;
  }
  i32 deserialized_i32 = 0;
  if (bytes[0] == PACK109_I32) {
    int ix = 1;
    for(int i = 3; i >= 0; i--) {
      int to_shift = 8 * i;
      i32 shifted = (i32)bytes[ix] << to_shift;
      deserialized_i32 = deserialized_i32 | shifted;
      ix++;
    }
    return deserialized_i32;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_I64
// ----------------------------------------

vec pack109::serialize(i64 item) {
  vec bytes;
  bytes.push_back(PACK109_I64);
  i64 mask = MASK8;
  for(int i = 7; i >= 0; i--) {
    int to_shift = 8 * i;
    i64 shifted = item >> (8 * i);
    bytes.push_back((u8)(item & mask));
  }
  return bytes;
}

i64 pack109::deserialize_i64(vec bytes) {
  if (bytes.size() < 9) {
    throw;
  }
  i64 deserialized_i64 = 0;
  if (bytes[0] == PACK109_I64) {
    int ix = 1;
    for(int i = 7; i>= 0; i--) {
      int to_shift = 8 * i;
      i64 byte = (i64)bytes[ix];
      deserialized_i64 = deserialized_i64 | byte << to_shift;
      ix++;
    }
    return deserialized_i64;
  } else {
    throw;
  }
}

// ----------------------------------------
// PACK109_S8 and PACK109_S16
// ----------------------------------------

// We can handle S8 and S16 in a single function by checking the length of the
// input string. If it's fewer than 256 characters, we can output a serialized
// S8. If it's up to 2^16, we can output a serialized S16.

vec pack109::serialize(string item) {
  vec bytes;
  if (item.size() < 256) {
    bytes.push_back(PACK109_S8);
    bytes.push_back((u8)item.size());
    // Push each byte of the string onto the vector
    for (int i = 0; i < item.size(); i++) {
      bytes.push_back(item[i]);
    }
  } else if (item.size() < 65536) {
    bytes.push_back(PACK109_S16);
    u32 string_length = (u32)item.size();
    // Push the first byte of the length onto the vector
    bytes.push_back((u8)(string_length >> 8));
    // Push the second byte of the length onto the vector 
    bytes.push_back((u8)(string_length));
    // Push each byte of the string onto the vector
    for (int i = 0; i < item.size(); i++) {
      bytes.push_back((u8)item[i]);
    }
  } else {
    throw;
  }
  return bytes;
}

string pack109::deserialize_string(vec bytes) {
  if(bytes.size() < 3) {
    throw;
  }
  string deserialized_string = "";
  if(bytes[0] == PACK109_S8) {
    // The string length is byte 1
    int string_length = bytes[1];
    // The string starts at byte 2
    for(int i = 2; i < (string_length + 2); i++) {
      deserialized_string += bytes[i];
    }
  }
  else if(bytes[0]==PACK109_S16) {
    // Reconstruct the string length from bytes 1 and 2
    int string_length = (bytes[1]<<8) | bytes[2];
    // The string starts at byte 3
    for(int i = 3; i < (string_length + 3); i++) {
      deserialized_string += bytes[i];
    }
  }
  return deserialized_string;
}
  
// ----------------------------------------
// PACK109_A8 and PACK109_A16
// ----------------------------------------

// Each of hte following functions will be very similar. There are two cases
// for the serialize function: one for the x8 and one for the x16 formats.
// In each function, we can leverage the serde functions written above
// to convert items and vectors from one form to another.

vec pack109::serialize(std::vector<u8> item) {
  vec bytes;
  if (item.size() < X8) {
    bytes.push_back(PACK109_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++) {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++) {
        bytes.push_back(temp[j]);
      }
    }
  } else if (item.size() < X16) {
    bytes.push_back(PACK109_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++) {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  } else {
    throw;
  }
  return bytes;
}

vec pack109::serialize(std::vector<u64> item) {
  vec bytes;
  if (item.size() < X8) {
    bytes.push_back(PACK109_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++) {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++) {
        bytes.push_back(temp[j]);
      }
    }
  } else if (item.size() < X16) {
    bytes.push_back(PACK109_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++) {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  } else {
    throw;
  }
  return bytes;
}

vec pack109::serialize(std::vector<string> item) {
  vec bytes;
  if (item.size() < X8) {
    bytes.push_back(PACK109_A8);
    u8 size = (u8)item.size();
    bytes.push_back(size);
    for (int i = 0; i < item.size(); i++) {
      vec temp = serialize(item[i]);
      for (int j = 0; j < temp.size(); j++) {
        bytes.push_back(temp[j]);
      }
    }
  } else if (item.size() < X16) {
    bytes.push_back(PACK109_A16);
    u32 item_length = (u32)item.size();
    bytes.push_back((u8)(item_length >> 8));
    bytes.push_back((u8)(item_length));
    for (int i = 0; i < item.size(); i++) {
      vec elem = serialize(item[i]);
      bytes.insert(end(bytes), begin(elem), end(elem));
    }
  } else {
    throw;
  }
  return bytes;
}

std::vector<u8> pack109::deserialize_vec_u8(vec bytes) {
  if(bytes.size() < 3) {
    throw;
  }
  int el_size = 2;
  std::vector<u8> result;
  if(bytes[0] == PACK109_A8) {
    int size = el_size * bytes[1];
    for (int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  } else if(bytes[0] == PACK109_A16) {
    int size = el_size * (((int)bytes[1])<<8 | (int)bytes[2]);
    for(int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i + 1, i + el_size);
      u64 element = deserialize_u8(sub_vec);
      result.push_back(element);
    }
  }
  return result;
}

std::vector<u64> pack109::deserialize_vec_u64(vec bytes) {
  if(bytes.size() < 3) {
    throw;
  }
  int el_size = 9;
  std::vector<u64> result;
  if(bytes[0] == PACK109_A8) {
    // Each u64 element is 9 bytes long (8 bytes payload + 1 tag byte), so
    // the total size of the payload is size byte (byte 1) multiplied by 9
    int size = el_size * bytes[1];
    // The payload starts at byte 2. We increment by 9 bytes every
    // iteration
    for (int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i, i + el_size);
      u64 element = deserialize_u64(sub_vec);
      result.push_back(element);
    }
  } else if(bytes[0] == PACK109_A16) {
    int size = el_size * (((int)bytes[1])<<8 | (int)bytes[2]);
    for(int i = 2; i < (size + 2); i += el_size) {
      vec sub_vec = slice(bytes, i + 1, i + el_size);
      u64 element = deserialize_u64(sub_vec);
      result.push_back(element);
    }
  }
  return result;
}

vec pack109::serialize(struct File item) {
  vec bytes;
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x1); // 1 k/v pair
  // The key is "File"
  vec file = serialize("File");
  bytes.insert(end(bytes), begin(file), end(file));
  // The value is an m8
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x2); // 3 k/v pairs
  // k/v 1 is "name"
  vec namek = serialize("name");
  bytes.insert(end(bytes), begin(namek), end(namek));
  vec namev = serialize(item.name);
  bytes.insert(end(bytes), begin(namev), end(namev));

  vec bytek = serialize("bytes");
  bytes.insert(end(bytes), begin(bytek), end(bytek));
  vec bytev = serialize(item.bytes);
  bytes.insert(end(bytes), begin(bytev), end(bytev));
  return bytes;
}


struct File pack109::deserialize_file(vec bytes) {
  if (bytes.size() < 10) {
    throw;
  }
  vec file_slice = slice(bytes, 2, 7);
  string file_string = deserialize_string(file_slice);
  if (file_string != "File") throw;
  u8 name_len = bytes[17];
  vec namev = slice(bytes, 16, 16 + name_len + 1);
  string name = deserialize_string(namev);

  u8 bytes_start = 16 + name_len + 1 + 8;
  vec bytesv = slice(bytes, bytes_start, bytes.size() - 1);
  std::vector<u8> bytesOutput = deserialize_vec_u8(bytesv);
  struct File deserialized_file = {name, bytesOutput};
  return deserialized_file;
}
//serialize struct Request for sending the requested file
vec pack109::serialize(struct Request item) {
  vec bytes;
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x1); // 1 k/v pair
  // The key is "Request"
  vec req = serialize("Request");
  bytes.insert(end(bytes), begin(req), end(req));
  // The value is an m8
  bytes.push_back(PACK109_M8);
  bytes.push_back(0x1); // 2 k/v pairs
  // k/v 1 is "name"
  vec namek = serialize("name");
  bytes.insert(end(bytes), begin(namek), end(namek));
  vec namev = serialize(item.name);
  bytes.insert(end(bytes), begin(namev), end(namev));
  return bytes;
}

struct Request pack109::deserialize_request(vec bytes) {
  if (bytes.size() < 12) {
    throw;
  }
  vec request_slice = slice(bytes, 2, 10);
  string request_string = deserialize_string(request_slice);
  if (request_string != "Request") throw;
  u8 name_len = bytes[20];
  vec namev = slice(bytes, 19, 19 + name_len + 1);
  string name = deserialize_string(namev);
  struct Request deserialized_struct = {name};
  return deserialized_struct;
}

void pack109::printVec(vec &bytes) {
  printf("[ ");
  int size = bytes.size();
  for (int i = 0; i < size; i++) {
    printf("%x ", bytes[i]);
  }
  printf("]\n");
}
/*----------- Encryption & Decryption with the KEY ---------------- */
void pack109::encryptVec(vec &bytes) {
  int size = bytes.size();
  for(int i = 0; i < size; i++) {
    bytes[i] = bytes[i] ^ KEY;
  }
}

void pack109::decryptVec(vec &bytes) {
  int size = bytes.size();
  for(int i = 0; i < size; i++) {
    bytes[i] = bytes[i] ^ KEY;
  }
}


/* ---- Reading Bytes From File Code ------------------------------ */
vec pack109::readBytesFromFile(string filePath) {
  FILE* f = fopen(filePath.c_str(),"rb");
  if (f == NULL) {
    perror("ERROR locating the file");
    exit(1);   
  }
  enum { SIZE = 100};
  vec mDataBuffer;
  vec buf(SIZE);
  fread(&buf[0], 1, SIZE, f);

  size_t bytesAvailable = 300000;
  size_t toRead = 300000;

  std::ifstream inf(filePath.c_str());
  if(inf) {
    mDataBuffer.resize(bytesAvailable);
    inf.read((char*)(&mDataBuffer[0]), toRead );
    size_t counted;
    if(inf.gcount() > 0) 
      counted = inf.gcount();
    else {
      perror("ERROR counting bytes");
      exit(1);
    }
    mDataBuffer.resize(counted) ;
    return mDataBuffer;
  }
  else {
    fprintf(stderr,"ERROR, Failed the read from the file");
    exit(0);
  }
}


/* ----- RECEIVING THE REQUESTED FILE FROM SOCKET ----- */
struct File pack109::requestFile(int sockfd) {
  vec buffer(300000);
  int n = recv(sockfd, buffer.data(), buffer.size(), 0);
  if(n == 0) {
    perror("ERROR requesting file");
    exit(0);
  }
  buffer.resize(n);
  decryptVec(buffer);
  File receivedFile = deserialize_file(buffer);
  return receivedFile;
}

