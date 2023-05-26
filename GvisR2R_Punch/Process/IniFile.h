#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>
using namespace std;

class CIniFile
{
public:
	struct Record
	{
		string Comments;
		char Commented;
		string Section;
		string Key;
		string Value;
	};
	vector<Record> m_vecRec;

	enum CommentChar
	{
		Pound = '#',
		SemiColon = ';'
	};

	CIniFile(void);
	CIniFile(string FileName);
	//CIniFile(CString FileName);
	virtual ~CIniFile(void);

	bool m_bLoaded, m_bChanged;
	string m_strFileName;

	string Content();
	vector<string> GetSectionNames();
	vector<Record> GetSection(string SectionName);
	bool RecordExists(string KeyName, string SectionName);
	bool SectionExists(string SectionName);
	vector<CIniFile::Record> GetRecord(string KeyName, string SectionName);
	string GetValue(string KeyName, string SectionName);
	bool SetValue(string KeyName, string Value, string SectionName);
	bool RenameSection(string OldSectionName, string NewSectionName);
	bool CommentRecord(CommentChar cc, string KeyName, string SectionName);
	bool UnCommentRecord(string KeyName, string SectionName);
	bool CommentSection(char CommentChar, string SectionName);
	bool UnCommentSection(string SectionName);
	bool DeleteRecord(string KeyName, string SectionName);
	bool DeleteSection(string SectionName);
	bool SetSectionComments(string Comments, string SectionName);
	bool SetRecordComments(string Comments, string KeyName, string SectionName);
	bool Sort(bool Descending);
	bool AddSection(string SectionName);
	bool Create();

	void StringToChar(CString str, char* pCh);
	CString CharToString(const char *szStr);

private:
	vector<CIniFile::Record> GetSections();
	bool Load(string FileName);
	//bool Load(CString sPath);
	bool Save();

	struct RecordSectionIs : std::unary_function<Record, bool>
	{
		std::string section_;

		RecordSectionIs(const std::string& section): section_(section){}

		bool operator()( const Record& rec ) const
		{
			return rec.Section == section_;
		}
	};

	struct RecordSectionKeyIs : std::unary_function<Record, bool>
	{
		std::string section_;
		std::string key_;

		RecordSectionKeyIs(const std::string& section, const std::string& key): section_(section),key_(key){}

		bool operator()( const Record& rec ) const
		{
			return ((rec.Section == section_)&&(rec.Key == key_));
		}
	};

	struct AscendingSectionSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Section < End.Section;
		}
	};

	struct DescendingSectionSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Section > End.Section;
		}
	};

	struct AscendingRecordSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Key < End.Key;
		}
	};

	struct DescendingRecordSort
	{
		bool operator()(Record& Start, Record& End)
		{
			return Start.Key > End.Key;
		}
	};
};