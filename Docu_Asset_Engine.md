# Asset Engine — Documentation

## Table of Contents

- [HTN Lib](#htn-lib)
  - [Data Storage Format](#data-storage-format)
  - [File class Guide](#file-class-guide)
  - [Archive class Guide](#archive-class-guide)
  - [MatGF4x4 class Guide](#matgf4x4-class-guide)
  - [Compress class Guide](#compress-class-guide)
  - [Crypt class Guide](#crypt-class-guide)
  - [CRC class Guide](#crc-class-guide)
  - [CommandLine class Guide](#commandline-class-guide)
  - [Use HTN Lib in terminal](#use-htn-lib-in-terminal)
- [Asset Engine Lib](#asset-engine-lib)
  - [Description](#description-1)
  - [Data Storage Format](#data-storage-format-1)
  - [AssetManager Class Guide](#assetmanager-class-guide)

---

# HTN Lib

## Data Storage Format

![Archive structure](archive_structure.png)

### Description

The archives are built following a precise structure.

- First we have the **Header**. Its goal is to give information about the footers which are at the end of the archive. In it we will find the compression info of them and the position of the first one. If the archive contains no file, the first position will be its own size (28 B by default).
- Next we have the **files** stored in the archive. They are all compressed alone and it allows a memory usage diminution except for the small files which may take more space.
- Finally we have the **file footers**. Each of them contains its associated file name, position in the archive and the compression information of it. It also stores the next footer position. This information allows us to calculate the version of the footer (length between footer position and next one (68 B by default)). On the final footer, the next footer position is set to 0.

> **NOTE :** It has to be mentioned but the Archive structure can be damaged if adding empty files.

### Compression Info

**CompressionInfo**

| NAME               | TYPE           | OFFSET (B) |
|:------------------:|:--------------:|:----------:|
| `crc32`            | uint32_t (4 B) | 0          |
| `compressedSize`   | uint64_t (8 B) | 4          |
| `decompressedSize` | uint64_t (8 B) | 12         |

CompressionInfo is a structure used in HTNLib to contain the information about the compression of a file.

The **crc32** field contains the CRC32 value of the file which is proper to each data set. It is used to verify if the decompression corrupted or damaged datas and is calculated before the compression.

The **compressedSize** field contains the size of the data compressed. It allows you to read the right size for the files in the archive instead of looking at the next fileFooter.

The **decompressedSize** field contains the size of the data decompressed. Used to allocate the right size in memory when decompressing the compressed data. Set right before compression.

### Archive Header

**ArchiveHeader**

| NAME              | TYPE                   | OFFSET (B) |
|:-----------------:|:----------------------:|:----------:|
| `compressionInfo` | CompressionInfo (20 B) | 0          |
| `firstFooterPos`  | uint64_t (8 B)         | 20         |

The Archive Header is the first element of an HTN archive.

The **compressionInfo** field contains the information about the fileFooters which are compressed together when the archive file isn’t being used.

The **firstFooterPos** field contains the position in Bytes of the first fileFooter. When the archive is empty the value is his own size (28 B). If the value is less, the archive is corrupted or contains an error.

### File Footer

**FileFooter**

| NAME              | TYPE                   | OFFSET (B) |
|:-----------------:|:----------------------:|:----------:|
| `compressionInfo` | CompressionInfo (20 B) | 0          |
| `dataPos`         | uint64_t (8 B)         | 20         |
| `name`            | char[32] (32 B)        | 28         |
| `nextFooterPos`   | uint64_t (8 B)         | 60         |

The File Footer is placed at the end of the archive. Its utility is to contain information about the associated file.

The **compressionInfo** field contains the information about the file which is compressed.

The **dataPos** field contains the position of the first Byte of the associated file in the archive file.

The **name** field contains the name of the file. The usage of this field is to recognize different files and is set based on the path given when added in the archive.

The **nextFooterPos** field contains the position in Bytes of the next fileFooter.
Travelling across the footers is based on a chained-list using this field. It can also be used to estimate the size of the footer which can change between versions (minimal size is 68 B).

---

## File class Guide

### Member values

#### `std::string mFilePath`

mFilePath contains the path given to create the instance. This path is used to open and get the file size of the file pointed by it.

#### `size_t mNameBegin`

mNameBegin contains the index of which element is the last `/` or `\` to find where the file name begins.

#### `uint64_t mFileSize`

mFileSize contains the size of the associated file the last time an ExtractData or SaveData operation was performed.

#### `bool mIsNew`

mIsNew says if the file associated was created at the run of this instance or not. If not, the file was already present on the hard drive.

#### `std::vector<uint8_t> mData`

mData contains the last ExtractData operation performed. It can be edited using the GetData method but I don’t recommend doing it for safety reasons.

### Constructor

#### `File::File(std::string filePath)`

File instances are created only with a **filePath**. Using this constructor will create the file if none is found. The name of the file is filled by scraping from back to front in **filePath** until it finds a `/` or `\`. If the **filePath** ends with `/` or `\` it crashes.

### Methods

#### `bool File::IsNew()`

IsNew is a method of the File class used to know if the file was created at this run or not. The method returns the **mIsNew** value.

#### `std::vector<uint8_t>& File::GetData()`

GetData is a method of the File class used to get a reference to the data vector contained in the instance (**mData**). This vector is modified when reading using `File::ExtractData` method.

#### `std::string File::GetName()`

GetName is a method of the File class used to get the name associated with the file (containing the extension). The name is guessed when creating the instance by scraping from back to front until it finds a `/` or `\`.

#### `void File::ExtractData()`

ExtractData is a method of the File class used to extract all the data contained inside the associated file. Extraction will be placed in the **mData** vector of the instance. Previous results will be overwritten.

#### `void File::ExtractData(offset_t beginPoint, size_t extractionSize)`

ExtractData is a method of the File class used to extract part of the data contained inside the associated file at a certain **beginPoint** with a certain **extractionSize**. Extraction will be placed in the **mData** vector of the instance. Previous results will be overwritten. If **extractionSize** is bigger than the fileSize, it will be reduced to the fileSize.

#### `void File::ExtractData(offset_t beginPoint, size_t extractionSize, std::vector<uint8_t>& output)`

ExtractData is a method of the File class used to extract part of the data contained inside the associated file at a certain **beginPoint** with a certain **extractionSize**. Extraction will be placed in the **output** vector which will be resized to size of **extractionSize**. **output** data will be overwritten. If **extractionSize** is bigger than the fileSize, it will be reduced to the fileSize.

#### `template <typename T> T File::ExtractData(uint64_t beginPoint = 0)`

ExtractData is a method of the File class used to extract part of the data contained inside the associated file at a certain **beginPoint** on the size of **T**. Extraction will be returned. If the size of **T** is bigger than the fileSize, it will return an empty value of type **T**.

#### `template <typename T> void File::ExtractData(T& outputValue, uint64_t beginPoint = 0)`

ExtractData is a method of the File class used to extract part of the data contained inside the associated file at a certain **beginPoint** on the size of **T**. Extraction will replace the value in **outputValue**. If the size of **T** is bigger than the fileSize, it will return an empty value of type **T**.

#### `void File::ReleaseData()`

ReleaseData is a method of the File class used to empty the **mData** vector of the instance.

#### `void File::SaveData(bool adding = false, std::string outputPath = "")`

SaveData is a method of the File class used to save the data contained in the **mData** vector. If **adding** is true, the content will be added at the end of the file. If **adding** is false, it will be overwritten from the beginning of the file until everything is written (file expands if required but never reduced). The **outputPath** parameter is meant to save in another file (created if required) the **mData** vector. If **outputPath** is empty, it will save in the associated file with the same parameters.

#### `void File::SaveData(std::vector<uint8_t>& rData, bool adding = false, std::string outputPath = "")`

SaveData is a method of the File class used to save the data contained in the **rData** vector. If **adding** is true, the content will be added at the end of the file. If **adding** is false, it will be overwritten from the beginning of the file until everything is written (file expands if required but never reduced). The **outputPath** parameter is meant to save in another file (created if required) the **rData** vector. If **outputPath** is empty, it will save in the associated file with the same parameters.

#### `void File::SaveData(std::vector<uint8_t>& rData, uint64_t filePosition = 0, std::string outputPath = "")`

SaveData is a method of the File class used to save the data contained in the **rData** vector. **filePosition** parameter is to paste the **rData** vector data at the given position overwriting the content and can expand the file size if necessary. The **outputPath** parameter is meant to save in another file (created if required) the **rData** vector. If **outputPath** is empty, it will save in the associated file with the same parameters.

#### `void File::SaveData(char* rData, uint64_t dataSize, uint64_t filePosition = 0, std::string outputPath = "")`

SaveData is a method of the File class used to save the data at the **rData** pointer on the **dataSize** length. **filePosition** parameter is to paste the **rData** data at the given position overwriting the content and can expand the file size if necessary. The **outputPath** parameter is meant to save in another file (created if required) the **rData**. If **outputPath** is empty, it will save in the associated file with the same parameters.

#### `template <typename T> void File::SaveData(const T& data, uint64_t filePosition = 0, std::string outputPath = "")`

SaveData is a method of the File class used to save the **data** at the **filePosition**. **filePosition** parameter is to paste the **data** at the given position overwriting the content and can expand the file size if necessary. The **outputPath** parameter is meant to save in another file (created if required) the **data**. If **outputPath** is empty, it will save in the associated file with the same parameters.

#### `void File::RemoveAt(uint64_t offset, uint64_t size)`

RemoveAt is a method of the File class used to remove a part of the associated file without leaving space. The removed part starts at the **offset** and its length is of **size**. If the **size**, starting from **offset**, is bigger than the file size, it stops at the end of the file. If the **offset** is bigger than the file size, it returns without moving anything.

---

## Archive class Guide

### Member values

#### `File mFile`

mFile is the link between the instance of the class and the Archive file on the hard drive. It is used to read or write in it. When creating the class instance, if the Archive file does not exist, it is created following the path given.

#### `std::string mArchivePath`

mArchivePath is kept in the instance of the class due to the edition of the Archive file. Each modification changes its size and this size is used multiple times to know the footers section size.

#### `ArchiveHeader mHeader`

mHeader is used to keep the header of the Archive file in dynamic memory. Its utility is to know where to write or read the footers and have the compression information of them.

#### `int mOpenDepth`

mOpenDepth utility is to know when the footers are compressed or not in the Archive file. Edition and read need them decompressed but out of these function we want to keep them compressed.

### Constructor

#### `Archive::Archive(std::string archivePath)`

Archive instances are created only with a path. Using this constructor will create the file if none is found and save the basic header in it at position 0. If one is found, it will open it and extract the header present in it.

> **NOTE :** there is no verification of the presence of the header. Opening another kind of file like PNG will be the same as an archive file.

### Methods

#### `void Archive::AddFile(std::vector<uint8_t>& fileData, std::string name)`

AddFile is a method of the Archive class used to add data in the Archive file. This override of the function will take the **fileData** vector and paste its compressed version as the last file. It will also create the corresponding file footer. The file identifier in the archive file will be the **name** given. If the file name is already used in the archive file used, it will erase the previous version and paste the given one.

#### `void Archive::AddFile(File& file)`

AddFile is a method of the Archive class used to add data in the Archive file. This override of the function will read the data in the File instance associated file and call the `Archive::AddFile(std::vector<uint8_t>& fileData, std::string name)` override of the method.

#### `void Archive::AddFile(std::string filePath)`

AddFile is a method of the Archive class used to add data in the Archive file. This override of the function will open the **filePath** using the File class and call the `void Archive::AddFile(File& file)` override of the method.

#### `void Archive::RemoveFile(std::string fileName)`

RemoveData is a method of the Archive class used to remove data in the Archive file. This override of the function will first search if the **fileName** is present inside the footers. If it is in, it will remove the data and the footer using `File::RemoveAt` and will refresh the header and footer's position values. If the removed file is the last one, it will set the last footer nextFooterPos value to 0.

#### `void Archive::RemoveFile(File& file)`

RemoveData is a method of the Archive class used to remove data in the Archive file. This override of the function will get the name inside the **file** instance and call the `void Archive::RemoveFile(std::string fileName)`.

#### `void Archive::ReadFile(std::string fileName, std::vector<uint8_t>& outData)`

ReadFile is a method of the Archive class used to read data in the Archive file. This method will first search if the **fileName** is present inside the footers. If it is in, it will copy its value compressed and decompress it in **outData**. If not, **outData** will be the same as before the call. **outData** needs also to be initialized but not necessarily the size of the decompressed data. All data in it will be overridden.

#### `std::vector<std::string> GetFileNames()`

GetFileNames is a method of the Archive class used to read data in the Archive file. This method will scrape the footers to get each file name and return a vector containing all of them.

---

## MatGF4x4 class Guide

This class is only used for the AES encryption with the concept of GF(2^8) calculations with matrix of bytes.

---

## Compress class Guide

### Member values

#### `unsigned long current_comp_size`

The size of the compressed file, this value is filled when you call `Compress::GetDeflatted()`.

### Methods

#### `std::string Crypt::GetDeflatted(char* input, size_t size)`

Return a string which contains the compressed data of **input\*** with the deflate function of the zlib library. **input\*** is the content of the file you want to compress. **size** is the quantity of data in **input\***.

#### `std::string Crypt::GetInflatted(char* input, size_t size)`

Return a string which contains the decompressed data of **input\*** with the inflate function of the zlib library. **input\*** is the content of the compressed file you want to uncompress. **size** is the quantity of data in **input\***.

#### `uint32_t Crypt::GetDeflattedBound(char* input, size_t size)`

Return the size of the compressed file without compressing it. **input\*** is the content of the file you want to know how much size it will take after the compression. **size** is the quantity of data in **input\***.

---

## Crypt class Guide

### Member values

#### `MatGF4x4 mAuxiliar`

A hardcoded matrix which contains value that will be multiplied by the columns of the data matrices for encryption.

#### `MatGF4x4 mInvAuxiliar`

The inverse matrix of mAuxiliar which contains value that will be multiplied by the columns of the data matrices for decryption.

### Methods

#### `void Crypt::InitAuxiliar()`

Init the mAuxiliar and mInvAuxiliar, you don’t have to call it by yourself.

#### `std::string Crypt::AES_Encrypt(char* data, size_t size, std::string key)`

Return a string which contains the encrypted data of **data\*** by the given **key**.

Because of the encryption method, the data need to be rounded to a multiple of 16, so the encrypted size can be slightly heavier if it wasn’t the case.

#### `std::string Crypt::AES_Decrypt(char* data, size_t encrypted_size, std::string key)`

Return a string which contains the decrypted data of **data\*** by the given **key**. You need to decrypt an encrypted file with the same key to have the original file.

The decrypted file will be also rounded to a multiple of 16 with empty datas at the end, it can be optimize in the future to fix it.

---

## CRC class Guide

```cpp
typedef uint32_t CRC32;
```

### Member values

#### `CRC32 mTable[256]`

Table used to calculate the crc32, you don’t have to edit this member.

### Methods

#### `void Crc::Init()`

Init mTable[256], you don’t have to call it by yourself.

#### `CRC32 Crc::Get(const uint8_t data[], size_t data_length)`

Generate a CRC32 value based of the data you gave, you can use it twice to verify the integrity of a file, you call it the first time when writing it and store it, you call it the second time when reading it, you compare the 2 values. If the 2 values aren’t the same, the file has a problem in it, it’s different.

---

## CommandLine class Guide

This class isn’t made for adding it anywhere, this is an internal class for handling the commands in the terminal console of HtnE.exe.
You don’t have to use it.

---

## Use HTN Lib in terminal

To use the HTN Lib using the terminal you need to call it, add a command and fill the parameters.

### Calling

> **NOTE :** the following example is when terminal is opened from the HTN.exe file location

```bash
./HTN.exe {params}
```

### Commands

#### `createArchive {archivePath}`

Create a new archive, specify the path and the name you want with the `.htn` extension.

#### `addFiles {archivePath} {filePath} … {otherFilePaths}`

Add files to the archive, you can put multiple files if you want.

#### `removeFiles {archivePath} {filePath} … {otherFilePaths}`

Remove files from the archive based on their names, you can put multiple files if you want.

#### `extractFile {archivePath} {fileName} {outDir}`

Extract a file from the archive based on the name, you need to give a valid folder that exists for OutDir.

#### `extractAllFiles {archivePath} {outDir}`

Extract all the files from the archive, you need to give a valid folder that exists for OutDir.

#### `encrypt {filePath} {key} {outDir = optional}`

Encrypt a given file with the AES method (without the complex key generation) with a key.
If you put an outDir, the file will be duplicated and will have `encrypted_` in front of its name.
Otherwise it will overwrite the current file !

#### `decrypt {filePath} {key} {outDir = optional}`

Decrypt a given file with the AES method (without the complex key generation) with a key.
If you want it to work, decrypt an encrypted file with the same key you gave to encrypt it.
If you put an outDir, the file will be duplicated and will have `decrypted_` in front of its name.
Otherwise it will overwrite the current file !

### Examples

> **NOTE :** remove the `{name}` by the right parameter value you want :

```bash
./HTN.exe createArchive "test.htn"
./HTN.exe addFiles "test.htn" "res/truc.bmp" "res/other.bmp"
./HTN.exe encrypt "test.htn" "verycoolkey" "crypted/"
```

---

# Asset Engine Lib

## Description

The Asset Engine Lib is made to use the HTN Lib in a presentation way. The Asset Engine Lib use the Archive File from HTN to store a custom file type. This type is the **htn_img** which is a generic image format. It is heavier than BMP or TGA (except when alpha is present on TGA) but the goal is to have generic format instead of memory usage.

> **NOTE :** Currently, the Asset Engine Lib only supports the BMP and TGA format for the images imported.

## Data Storage Format

![htn_img structure](htn_img_structure.png)

### HTN_Image_Header

**HTN_Image_Header**

| NAME            | TYPE            | OFFSET (B) |
|:---------------:|:---------------:|:----------:|
| `imageName`     | char[32] (32 B) | 0          |
| `width`         | uint32_t (4 B)  | 32         |
| `height`        | uint32_t (4 B)  | 36         |
| `dataSize`      | uint64_t (8 B)  | 40         |
| `offsetToData`  | uint64_t (8 B)  | 48         |
| *(empty space)* | void (44 B)     | 56         |

HTN_Image_Header is the header corresponding to the file header of the file type htn_img.

The **imageName** field contains the name given to the image. It can be different from the footerFile to give the image a custom name for when it’s decompressed.

The **width** and **height** field contains the dimensions of the image in pixels.

The **dataSize** field is the size of the pixel array. It is different from the footer to correspond to the file size minus the HTN_Image_Header. Storing the value removes the calculation when using it.

The **offsetToData** is the offset from the header to get access to the pixel array. Storing it is in accordance with the (empty space) shown in the tab below. The reason why it is here is to handle the possibility of an addition of information in the header.

### HTN_Pixel

**HTN_Pixel**

| NAME | TYPE          | OFFSET (B) |
|:----:|:-------------:|:----------:|
| `r`  | uint8_t (1 B) | 0          |
| `g`  | uint8_t (1 B) | 1          |
| `b`  | uint8_t (1 B) | 2          |
| `a`  | uint8_t (1 B) | 3          |

HTN_Pixel is a structure meant to be used when manipulating the pixels of the image. Each channel corresponds to a canal for a color argument (RGBA).

### HTN_Image_Data

**HTN_Image_Data**

| NAME     | TYPE                                  | OFFSET (B)                            |
|:--------:|:-------------------------------------:|:-------------------------------------:|
| `width`  | uint32_t (4 B)                        | 32                                    |
| `height` | uint32_t (4 B)                        | 36                                    |
| `pixels` | `std::vector<std::vector<HTN_Pixel>>` | sizeoff(HTN_Pixel \* width \* height) |

HTN_Pixel is a structure meant to store all the pixels in a 2D array of vectors. It is used to convert different types of files (BMP, TGA) into a universal, custom and readable file format.

---

## AssetManager Class Guide

### `AddImages(archivePath, std::vector<imgPaths>)`

addImages is a function that takes an image file or multiple with BMP or TGA extensions and load them into the chosen archive name. The function takes the image file name, checks which file type is provided, loads each header and the main pixel data from the path, reads the width and height, converts the BMP or TGA pixels into HTN_Pixel values, and creates the HTNimgdata structure.

We complete the header with the width, height, size, data offset, and name, place both the header and the main file data into an array of uint8_t, store all the data, and add it to the archive using AddFiles.

### `RemoveImages(archivePath, std::vector<imgNames>)`

RemoveImages is a function that takes an archive path and one or multiple image names which are in the archive to remove them.

### `LoadImage(archivePath, imageName)`

LoadImage is a function used to get an HTN_Image_Data structure from an archive by its name and the name of its archive. We get the uint8_t data array, which we use to populate the HTN_Header and HTN_Data. If the name is not in an archive, we return an empty HTN_Image_Data.
