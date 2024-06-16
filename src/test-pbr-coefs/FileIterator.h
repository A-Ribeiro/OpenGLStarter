#pragma once

#include <InteractiveToolkit/common.h>
#include <InteractiveToolkit/ITKCommon/StringUtil.h>
#include <InteractiveToolkit/ITKCommon/Path.h>
#include <InteractiveToolkit/Platform/platform_common.h>

struct Date {

	uint16_t wYear;
	uint16_t wMonth;
	uint16_t wDayOfWeek;
	uint16_t wDay;
	uint16_t wHour;
	uint16_t wMinute;
	uint16_t wSecond;
	uint16_t wMilliseconds;

	Date() {
		wYear = 0;
		wMonth = 0;
		wDayOfWeek = 0;
		wDay = 0;
		wHour = 0;
		wMinute = 0;
		wSecond = 0;
		wMilliseconds = 0;
	}

	Date(uint16_t wYear, uint16_t wMonth,
		uint16_t wDayOfWeek, uint16_t wDay,
		uint16_t wHour, uint16_t wMinute,
		uint16_t wSecond, uint16_t wMilliseconds) {
		this->wYear = wYear;
		this->wMonth = wMonth;
		this->wDayOfWeek = wDayOfWeek;
		this->wDay = wDay;
		this->wHour = wHour;
		this->wMinute = wMinute;
		this->wSecond = wSecond;
		this->wMilliseconds = wMilliseconds;
	}

	std::string formattedLocal()const {
		char result[64];
		snprintf(result, 64, "%.2i/%.2i/%.4i %.2i:%.2i:%.2i",
			wDay, wMonth, wYear,
			wHour, wMinute, wSecond);
		return result;
	}
};

struct FileInfo {
	std::string base_path;
	std::string full_path;
	std::string name;
	bool isDirectory;
	bool isFile;

	Date creationTime;
	Date lastWriteTime;

	uint64_t size;

	FileInfo() {
		isDirectory = false;
		isFile = false;
		size = UINT64_C(0);
	}
};

class Directory {
public:
	struct const_iterator {
		using iterator_category = std::input_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = FileInfo;
		using pointer = const FileInfo*;
		using reference = const FileInfo&;

		const_iterator() {
			memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
			hFind = INVALID_HANDLE_VALUE;
		}

		const_iterator(const std::string& base_path) {
			memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
			hFind = INVALID_HANDLE_VALUE;

			std::wstring w_base_path = ITKCommon::StringUtil::string_to_WString(base_path + "*");
			hFind = FindFirstFileW((LPCWSTR)w_base_path.c_str(), &findfiledata);
			if (hFind != INVALID_HANDLE_VALUE) {
				fileInfo.base_path = base_path;
				processCurrentValue(TRUE);
			}
			//else {
			//	printf("%s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
			//}
		}

		~const_iterator() {
			if (hFind != INVALID_HANDLE_VALUE) {
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
			}
		}

		const reference operator*() const { return fileInfo; }
		const pointer operator->() const { return &fileInfo; }

		// Prefix increment
		const_iterator& operator++() {
			
			if (hFind == INVALID_HANDLE_VALUE) {
				fileInfo = FileInfo();
				return;
			}

			BOOL next_valid = FindNextFileW(hFind, &findfiledata);
			processCurrentValue(next_valid);

			return *this;
		}

		// Postfix increment
		const_iterator operator++(int) { const_iterator tmp = *this; (*this)++; return tmp; }

		friend bool operator== (const const_iterator& a, const const_iterator& b) {
			return a.hFind == b.hFind &&
				a.fileInfo.full_path.compare(b.fileInfo.full_path) == 0;

		}
		friend bool operator!= (const const_iterator& a, const const_iterator& b) {
			return !(a == b);
		}

	private:

		void processCurrentValue(BOOL next_valid) {
			if (hFind == INVALID_HANDLE_VALUE)
				return;

			// skip . and ..
			while (next_valid &&
				(findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 &&
				(wcscmp(findfiledata.cFileName, L".") == 0 ||
					wcscmp(findfiledata.cFileName, L"..") == 0)) {
				next_valid = FindNextFileW(hFind, &findfiledata);
			}

			if (next_valid == FALSE) {
				FindClose(hFind);
				hFind = INVALID_HANDLE_VALUE;
				fileInfo = FileInfo();
			}
			else {
				fileInfo.isDirectory = (findfiledata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
				fileInfo.isFile = !fileInfo.isDirectory;
				fileInfo.name = ITKCommon::StringUtil::wString_to_String(findfiledata.cFileName);
				fileInfo.full_path = fileInfo.base_path + fileInfo.name;
				if (fileInfo.isDirectory)
					fileInfo.full_path += "/";

				// date processing
				SYSTEMTIME stUTC, stLocal;
				FileTimeToSystemTime(&findfiledata.ftLastWriteTime, &stUTC);
				SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
				fileInfo.lastWriteTime = Date(
					stLocal.wYear,
					stLocal.wMonth,
					stLocal.wDayOfWeek,
					stLocal.wDay,
					stLocal.wHour,
					stLocal.wMinute,
					stLocal.wSecond,
					stLocal.wMilliseconds);

				FileTimeToSystemTime(&findfiledata.ftCreationTime, &stUTC);
				SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);
				fileInfo.creationTime = Date(
					stLocal.wYear,
					stLocal.wMonth,
					stLocal.wDayOfWeek,
					stLocal.wDay,
					stLocal.wHour,
					stLocal.wMinute,
					stLocal.wSecond,
					stLocal.wMilliseconds);

				fileInfo.size =
					((uint64_t)findfiledata.nFileSizeHigh << 32)
					| (uint64_t)findfiledata.nFileSizeLow & UINT64_C(0xffffffff);
			}
		}

		const_iterator(const const_iterator& v) {
			memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
			hFind = INVALID_HANDLE_VALUE;

			fileInfo = v.fileInfo;
		}
		void operator=(const const_iterator& v) {
			memset(&findfiledata, 0, sizeof(WIN32_FIND_DATAW));
			hFind = INVALID_HANDLE_VALUE;

			fileInfo = v.fileInfo;
		}

		WIN32_FIND_DATAW findfiledata;
		HANDLE hFind;

		FileInfo fileInfo;

	};

	const_iterator begin() const { return const_iterator(base_path); }
	const_iterator end() const { return const_iterator(); }

	std::string base_path;

	Directory(const std::string& base_path = "./") {
		this->base_path = ITKCommon::Path::getAbsolutePath(base_path);
		ITKCommon::StringUtil::replaceAll(&this->base_path, "\\", "/");
		ITKCommon::StringUtil::replaceAll(&this->base_path, "/", "/");
		if (!ITKCommon::StringUtil::endsWith(this->base_path, "/"))
			this->base_path += "/";
	}



};

