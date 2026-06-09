/**
 * @file Main.cpp
 * @author silvio3105 (www.github.com/silvio3105)
 * @brief Main source file.
 * 
 * @copyright Copyright (c) 2026, silvio3105 (www.github.com/silvio3105)
 * 
 */

// ----- INCLUDE FILES
#include			<CLI11.hpp>

#include			<cstdint>
#include			<iostream>
#include			<fstream>


// ----- ENUMS
/**
 * @brief Enum class with endian values.
 * 
 */
enum class Endian_t : uint8_t
{
	Little = 0, /**< @brief Output is in little endian. */
	Big /**< @brief Output is in big endian. */
};

/**
 * @brief Enum class with hash algorithms.
 * 
 */
enum class Algorithm_t : uint8_t
{
	ModbusCRC16 /**< @brief Use ModbusCRC16. */
};


// ----- STRUCTS
/**
 * @brief Struct for input from arguments.
 * 
 */
struct Input_s
{
	std::string filePath = ""; /**< @brief Path to .bin file to process. */
	uint32_t hashOffset = 0; /**< @brief Hash word offset in file. */
	uint32_t sizeOffset = 0; /**< @brief Size word offset in file. */
	std::string preSalt = ""; /**< @brief Pre salt string. */
	std::string postSalt = ""; /**< @brief Post salt string. */

	Endian_t endian = Endian_t::Little; /**< @brief Output endian. */
	Algorithm_t algorithm = Algorithm_t::ModbusCRC16; /**< @brief Hash algorithm. */
	uint8_t alignment = 0; /**< @brief File size check divider. */
};


/**
 * @brief Struct with input file info.
 * 
 */
struct File_s
{
	uint32_t size = 0; /**< @brief File size in bytes. */
	uint32_t hash = 0; /**< @brief Calculated file hash. */
};


// ----- VARIABLES
static constexpr char version[] = "sPostBuild v1.0rc2"; /**< @brief Application version. */

static Input_s input; /**< @brief Input info from arguments. */
static File_s fileInfo; /**< @brief Input file info. */
static std::string tmpAlgorithm; /**< @brief Selected algorithm to use. Not in use. */


// ----- NAMESPACES
/**
 * @brief ModbusCRC namespace.
 * 
 */
namespace ModbusCRC
{
	// FUNCTION DEFINITIONS
	/**
	 * @brief Prepare output for calculation.
	 * 
	 * @param var Reference to calculate output variable.
	 */
	inline void init(uint32_t& var)
	{
		var = 0xFFFFFFFF;
	}

	/**
	 * @brief Calculate Modbus CRC.
	 * 
	 * @param output Reference to output variable.
	 * @param data Pointer to input data.
	 * @param len Size of \c data in bytes.
	 */
	void calculate(uint32_t& output, const void* data, uint32_t len)
	{
		/*
			8 x 32
			Modbus CRC16
			http://www.modbustools.com/modbus_crc16.htm
			poly: 0x8005 (reverted 0xA001)
			init: 0xFFFF
		*/

		static constexpr uint16_t lookup[] =
		{
			0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
			0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
			0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
			0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
			0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
			0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
			0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
			0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
			0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
			0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
			0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
			0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
			0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
			0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
			0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
			0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
			0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
			0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
			0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
			0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
			0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
			0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
			0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
			0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
			0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
			0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
			0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
			0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
			0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
			0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
			0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
			0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
		};

		uint8_t* bytes = (uint8_t*)data;
		uint8_t tmp = 0;

		while (len--)
		{				
			tmp = *bytes++ ^ output;
			output >>= 8;
			output ^= lookup[tmp];	  
		}
	}	
};


// ----- STATIC FUNCTION DEFINITIONS
/**
 * @brief Swap byte order.
 * 
 * @param output Pointer to output.
 * @param input Pointer to input.
 * @param n Number of bytes to swap.
 */
static inline void swapEndian(void* output, const void* input, const uint8_t n)
{
	for (uint8_t i = 0; i < n; i++)
	{
		((uint8_t*)output)[i] = ((uint8_t*)input)[(n - 1) - i];
	}
}

/**
 * @brief Get file size.
 * 
 * @param file Reference to file.
 * 
 * @return File size in bytes.
 */
static std::streamsize getFileSize(std::fstream& file)
{
	file.seekg(0, std::ios::end);
	const std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);
	return size;
}

/**
 * @brief Write size to the file.
 * 
 * @param file Reference to file handle.
 * 
 * @return \c 1 on fail.
 * @return \c 0 on success. 
 */
static int writeSize(std::fstream& file)
{
	uint8_t tmp[sizeof(fileInfo.size)];
	memcpy(tmp, &fileInfo.size, sizeof(tmp));

	if (input.endian == Endian_t::Big)
	{
		swapEndian(tmp, &fileInfo.size, sizeof(fileInfo.size));
	}

	file.seekp(input.sizeOffset);
	if (!file.write((const char*)tmp, sizeof(fileInfo.size)))
	{
		return 1;
	}

	return 0;
}

/**
 * @brief Calculate file hash.
 * 
 * @param file Reference to file handle.
 * @param output Reference to hash output.
 * 
 * @return \c 1 on fail.
 * @return \c 0 on success.
 */
static uint32_t getHash(std::fstream& file, uint32_t& output)
{
	ModbusCRC::init(output);

	// Insert pre salt
	if (input.preSalt.length())
	{
		std::cout << "Pre salt '" << input.preSalt << "'" << std::endl;
		ModbusCRC::calculate(output, &input.preSalt[0], input.preSalt.length());
	}

	// Before checksum
	file.seekg(0, std::ios::beg);
	std::vector<char> buffer(input.hashOffset);

	if (!file.read(buffer.data(), input.hashOffset))
	{
		std::cerr << "Read 1 fail" << std::endl;
		return 1;
	}
	ModbusCRC::calculate(output, buffer.data(), input.hashOffset);

	// After checksum
	const uint32_t size = fileInfo.size - (input.hashOffset + sizeof(fileInfo.hash));
	file.seekg(input.hashOffset + sizeof(fileInfo.hash), std::ios::beg);

	buffer.clear();
	buffer.resize(size);
	if (!file.read(buffer.data(), size))
	{
		std::cerr << "Read 2 fail" << std::endl;
		return 1;
	}
	ModbusCRC::calculate(output, buffer.data(), size);

	// Insert post salt
	if (input.postSalt.length())
	{
		std::cout << "Post salt '" << input.postSalt << "'" << std::endl;
		ModbusCRC::calculate(output, &input.postSalt[0], input.postSalt.length());
	}

	return 0;
}

/**
 * @brief Write hash to the file.
 * 
 * @param file Reference to file handle.
 * 
 * @return \c 1 on fail.
 * @return \c 0 on success. 
 */
static int writeHash(std::fstream& file)
{
	uint8_t tmp[sizeof(fileInfo.hash)];
	memcpy(tmp, &fileInfo.hash, sizeof(tmp));

	if (input.endian == Endian_t::Big)
	{
		swapEndian(tmp, &fileInfo.hash, sizeof(fileInfo.hash));
	}

	file.seekp(input.hashOffset);
	if (!file.write((const char*)tmp, sizeof(fileInfo.hash)))
	{
		return 1;
	}

	return 0;
}


// ----- FUNCTION DEFINITIONS
/**
 * @brief Application entry point.
 * 
 * @param argc Number of arguments.
 * @param argv Pointer to list of arguments.
 */
int main(int argc, char* argv[])
{
	// Prepare CLI parser
	CLI::App app{"\n\nsPostBuild tool for writing firmware size and its hash into build info\nCopyright (c) 2026, silvio3105 (www.github.com/silvio3105)"};
	app.set_version_flag("--version,-v", version);
	argv = app.ensure_utf8(argv);

	app.add_option("--file", input.filePath, "Path to .bin file to process")->required();
	app.add_option<uint32_t>("--hash-offset", input.hashOffset, "Hash word offset in the file")->required();
	app.add_option<uint32_t>("--size-offset", input.sizeOffset, "Size word offset in the file")->required();
	app.add_option<uint8_t>("--alignment", input.alignment, "File size divider");
	app.add_option("--algorithm", tmpAlgorithm, "Hash algorithm to use")->check(CLI::IsMember({"modbuscrc16"}));
	app.add_option("--pre-salt", input.preSalt, "Pre file salt string");
	app.add_option("--post-salt", input.postSalt, "Post file salt string");
	app.add_flag_callback("--big-endian", [&] { input.endian = Endian_t::Big; });

	CLI11_PARSE(app, argc, argv);
	input.algorithm = Algorithm_t::ModbusCRC16;

	// Open file
	std::fstream file(input.filePath, std::ios::binary | std::ios::in | std::ios::out);
	if (!file)
	{
		std::cerr << "File open fail" << std::endl;
		return 1;
	}

	// Get file size
	fileInfo.size = getFileSize(file);
	if (input.alignment)
	{
		if (fileInfo.size % input.alignment)
		{
			std::cerr << "File size not valid" << std::endl;
			return 1;
		}
	}

	// Validate offsets
	if ((input.hashOffset - sizeof(fileInfo.hash)) >= fileInfo.size)
	{
		std::cerr << "Hash offset not valid" << std::endl;
		return 1;
	}

	if ((input.sizeOffset - sizeof(fileInfo.size)) >= fileInfo.size)
	{
		std::cerr << "Size offset not valid" << std::endl;
		return 1;
	}	

	if (input.hashOffset == input.sizeOffset)
	{
		std::cerr << "Offset overlap" << std::endl;
		return 1;
	}

	// SOON: Add partial overlap check

	// Write size to file
	if (writeSize(file))
	{
		std::cerr << "Size write fail" << std::endl;
		return 1;
	}

	// Calculate hash
	if (getHash(file, fileInfo.hash))
	{
		std::cerr << "Hash fail" << std::endl;
		return 1;
	}

	if (writeHash(file))
	{
		std::cerr << "Hash write fail" << std::endl;
		return 1;
	}

	std::cout << "File '" << input.filePath << "', " << fileInfo.size << "B, hash " << std::hex << fileInfo.hash << std::endl;

	//std::cin.get();
	return 0;
}



// END WITH NEW LINE
