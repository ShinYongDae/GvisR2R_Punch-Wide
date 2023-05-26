#include "StdAfx.h"
#include "inifile.h"

CIniFile::CIniFile(void)													// Default constructor
{
	m_bLoaded = false;
	m_strFileName = "";
}

CIniFile::CIniFile(string FileName)													// Default constructor
{
	m_bLoaded = false;
	m_bChanged = false;
	m_vecRec.clear();
	if (Load(FileName))
	{
		m_bLoaded = true;
		m_strFileName = FileName;
	}
}
//CIniFile::CIniFile(CString FileName)													// Default constructor
//{
//	m_bLoaded = false;
//	m_bChanged = false;
//	m_vecRec.clear();
//	if (Load(FileName))
//	{
//		m_bLoaded = true;
//		m_strFileName = CT2CA(FileName);;
//	}
//}

CIniFile::~CIniFile(void)
{
	if (m_bLoaded)
		Save();
}

// A function to trim whitespace from both sides of a given string
void Trim(std::string& str, const std::string & ChrsToTrim = " \t\n\r", int TrimDir = 0)
{
    size_t startIndex = str.find_first_not_of(ChrsToTrim);
    if (startIndex == std::string::npos){str.erase(); return;}
    if (TrimDir < 2) str = str.substr(startIndex, str.size()-startIndex);
    if (TrimDir!=1) str = str.substr(0, str.find_last_not_of(ChrsToTrim) + 1);
}

//inline void TrimRight(std::string& str, const std::string & ChrsToTrim = " \t\n\r")
//{
//    Trim(str, ChrsToTrim, 2);
//}

//inline void TrimLeft(std::string& str, const std::string & ChrsToTrim = " \t\n\r")
//{
//    Trim(str, ChrsToTrim, 1);
//}

// A function to transform a string to uppercase if neccessary
//void UCase(string& str, bool ucase)
//{
//	if(ucase) transform(str.begin(), str.end(), str.begin(), toupper);
//}

bool CIniFile::Load(string FileName)
{
	int dwStart = GetTickCount();

	string s, s_0, s_1;														// Holds the current line from the ini file
	string CurrentSection;													// Holds the current section name

	ifstream inFile(FileName.c_str());										// Create an input filestream
	if (!inFile.is_open()) return false;									// If the input file doesn't open, then return
	m_vecRec.clear();														// Clear the content vector

	string comments = "";													// A string to store comments in

	while (!std::getline(inFile, s).eof())									// Read until the end of the file
	{
		Trim(s);															// Trim whitespace from the ends
		if (!s.empty())														// Make sure its not a blank line
		{
			Record r;														// Define a new record

			if ((s[0] == '#') || (s[0] == ';'))									// Is this a commented line?
			{
				if ((s.find('[') == string::npos) &&							// If there is no [ or =
					(s.find('=') == string::npos))							// Then it's a comment
				{
					comments += s + '\n';									// Add the comment to the current comments string
				}
				else {
					r.Commented = s[0];										// Save the comment character
					s.erase(s.begin());										// Remove the comment for further processing
					Trim(s);
				}// Remove any more whitespace
			}
			else r.Commented = ' ';										// else mark it as not being a comment

			if (s.find('[') != string::npos)									// Is this line a section?
			{
				s.erase(s.begin());											// Erase the leading bracket
				s.erase(s.find(']'));										// Erase the trailing bracket
				r.Comments = comments;										// Add the comments string (if any)
				comments = "";												// Clear the comments for re-use
				r.Section = s;												// Set the Section value
				r.Key = "";													// Set the Key value
				r.Value = "";												// Set the Value value
				CurrentSection = s;
			}

			if (s.find('=') != string::npos)								// Is this line a Key/Value?
			{
				r.Comments = comments;										// Add the comments string (if any)
				comments = "";												// Clear the comments for re-use
				r.Section = CurrentSection;									// Set the section to the current Section
				s_0 = s.substr(0, s.find('='));
				Trim(s_0);
				r.Key = s_0;												// Set the Key value to everything before the = sign
				s_1 = s.substr(s.find('=') + 1);
				Trim(s_1);
				r.Value = s_1;												// Set the Value to everything after the = sign
			}
			if (comments == "")												// Don't add a record yet if its a comment line
				m_vecRec.push_back(r);										// Add the record to content
		}
	}

	inFile.close();															// Close the file

	int dwEnd = GetTickCount();
	int dwElapsed = dwEnd - dwStart;

	return true;
}

//bool CIniFile::Load(CString sPath)
//{
//	//int dwStart = GetTickCount();
//
//	CFileFind cFile;
//	if (!cFile.FindFile(sPath))
//		return false;															// If the input file doesn't open, then return
//		//DeleteFile(sItsPath);
//
//	char FileName[MAX_PATH];
//	StringToChar(sPath, FileName);
//
//	char *FileData;
//	size_t nFileSize, nRSize;
//	//CString strFileData;
//
//	FILE *fp = NULL;
//	fp = fopen(FileName, "r");													// Open the file
//	if (fp != NULL)
//	{
//		fseek(fp, 0, SEEK_END);
//		nFileSize = ftell(fp);
//		fseek(fp, 0, SEEK_SET);
//
//		/* Allocate space for a path name */
//		FileData = (char*)calloc(nFileSize + 1, sizeof(char));
//
//		nRSize = fread(FileData, sizeof(char), nFileSize, fp);
//		//strFileData.Format(_T("%s"), CharToString(FileData));
//		fclose(fp);																// Close the file
//	}
//	else
//	{
//		//pView->MsgBox(_T("It is trouble to MakeItsFile."), MB_ICONWARNING | MB_OK);
//		return false;															// If the input file doesn't open, then return
//	}
//
//	string s, s_0, s_1;															// Holds the current line from the ini file
//	string CurrentSection;														// Holds the current section name
//
//	m_vecRec.clear();															// Clear the content vector
//
//	string comments = "";														// A string to store comments in
//
//	int dwStart = GetTickCount();
//
//	char cLine[MAX_PATH];
//	int i, line_start, line_end, line_ch;
//	line_start = 0; line_ch = 0;
//	for (i = 0; i < nFileSize; i++)												// Read until the end of the file
//	{
//		if (FileData[i] == '\n')
//		{
//			cLine[line_ch] = '\0';
//			s = (string)cLine;
//			line_ch = 0;
//
//			Trim(s);															// Trim whitespace from the ends
//			if (!s.empty())														// Make sure its not a blank line
//			{
//				Record r;														// Define a new record
//
//				if ((s[0] == '#') || (s[0] == ';'))									// Is this a commented line?
//				{
//					if ((s.find('[') == string::npos) &&							// If there is no [ or =
//						(s.find('=') == string::npos))							// Then it's a comment
//					{
//						comments += s + '\n';									// Add the comment to the current comments string
//					}
//					else {
//						r.Commented = s[0];										// Save the comment character
//						s.erase(s.begin());										// Remove the comment for further processing
//						Trim(s);
//					}// Remove any more whitespace
//				}
//				else r.Commented = ' ';											// else mark it as not being a comment
//
//				if (s.find('[') != string::npos)								// Is this line a section?
//				{
//					s.erase(s.begin());											// Erase the leading bracket
//					s.erase(s.find(']'));										// Erase the trailing bracket
//					r.Comments = comments;										// Add the comments string (if any)
//					comments = "";												// Clear the comments for re-use
//					r.Section = s;												// Set the Section value
//					r.Key = "";													// Set the Key value
//					r.Value = "";												// Set the Value value
//					CurrentSection = s;
//				}
//
//				if (s.find('=') != string::npos)								// Is this line a Key/Value?
//				{
//					r.Comments = comments;										// Add the comments string (if any)
//					comments = "";												// Clear the comments for re-use
//					r.Section = CurrentSection;									// Set the section to the current Section
//					s_0 = s.substr(0, s.find('='));
//					Trim(s_0);
//					r.Key = s_0;												// Set the Key value to everything before the = sign
//					s_1 = s.substr(s.find('=') + 1);
//					Trim(s_1);
//					r.Value = s_1;												// Set the Value to everything after the = sign
//				}
//				if (comments == "")												// Don't add a record yet if its a comment line
//					m_vecRec.push_back(r);										// Add the record to content
//			}
//
//		}
//		else
//		{
//			cLine[line_ch] = FileData[i];
//			line_ch++;
//		}
//
//	}
//
//	free(FileData);
//
//	int dwEnd = GetTickCount();
//	int dwElapsed = dwEnd - dwStart;
//
//	return true;
//}

bool CIniFile::Save()
{
	if (!m_bChanged)
		return true;

	int dwStart = GetTickCount();

	ofstream outFile(m_strFileName.c_str());								// Create an output filestream
	if (!outFile.is_open()) return false;									// If the output file doesn't open, then return

	for (int i = 0; i<(int)m_vecRec.size(); i++)							// Loop through each vector
	{
		outFile << m_vecRec[i].Comments;									// Write out the comments
		if (m_vecRec[i].Key == "")											// Is this a section?
			outFile << m_vecRec[i].Commented << "["
			<< m_vecRec[i].Section << "]" << endl;							// Then format the section
		else
			outFile << m_vecRec[i].Commented << m_vecRec[i].Key
			<< "=" << m_vecRec[i].Value << endl;								// Else format a key/value
	}

	outFile.close();														// Close the file

	int dwEnd = GetTickCount();
	int dwElapsed = dwEnd - dwStart;

	return true;
}

string CIniFile::Content()
{
	string FileName = m_strFileName;

	string s = "";															// Hold our return string

	if (m_bLoaded)															// Make sure the file loads
	{
		for (int i = 0; i<(int)m_vecRec.size(); i++)						// Loop through the content
		{
			if (m_vecRec[i].Comments != "") s += m_vecRec[i].Comments;		// Add the comments
			if (m_vecRec[i].Commented != ' ') s += m_vecRec[i].Commented;	// If this is commented, then add it
			if ((m_vecRec[i].Key == ""))									// Is this a section?
				s += '[' + m_vecRec[i].Section + ']';						// Add the section
			else s += m_vecRec[i].Key + '=' + m_vecRec[i].Value;			// Or the Key value to the return srting

			if (i != m_vecRec.size()) s += '\n';							// If this is not the last line, add a CrLf
		}
		return s;															// Return the contents
	}

	return "";
}


vector<string> CIniFile::GetSectionNames()
{
	vector<string> data;													// Holds the return data

	if (m_bLoaded)															// Make sure the file loads
	{
		for (int i = 0; i<(int)m_vecRec.size(); i++)						// Loop through the content
		{
			if (m_vecRec[i].Key == "")										// If there is no key value, then its a section
				data.push_back(m_vecRec[i].Section);						// Add the section to the return data
		}
	}

	return data;															// Return the data
}


vector<CIniFile::Record> CIniFile::GetSection(string SectionName)
{
	vector<Record> data;													// Holds the return data

	if (m_bLoaded)															// Make sure the file is loaded
	{
		for (int i=0;i<(int)m_vecRec.size();i++)							// Loop through the content
		{
			if((m_vecRec[i].Section == SectionName) &&						// If this is the section name we want
				(m_vecRec[i].Key != ""))									// but not the section name itself
				data.push_back(m_vecRec[i]);								// Add the record to the return data
		}
	}
	
	return data;															// Return the data
}


bool CIniFile::RecordExists(string KeyName, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(),
			m_vecRec.end(),
			CIniFile::RecordSectionKeyIs(SectionName, KeyName));			// Locate the Section/Key

		if (iter == m_vecRec.end()) return false;							// The Section/Key was not found
	}
	return true;															// The Section/Key was found
}


bool CIniFile::SectionExists(string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(),
			m_vecRec.end(),
			CIniFile::RecordSectionIs(SectionName));						// Locate the Section

		if (iter == m_vecRec.end()) return false;							// The Section was not found
	}
	return true;															// The Section was found
}

vector<CIniFile::Record> CIniFile::GetRecord(string KeyName, string SectionName)
{
	vector<Record> data;													// Holds the return data

	if (m_bLoaded)											// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(),
			m_vecRec.end(),
			CIniFile::RecordSectionKeyIs(SectionName, KeyName));			// Locate the Record

		if (iter == m_vecRec.end()) return data;								// The Record was not found

		data.push_back(*iter);												// The Record was found
	}
	return data;															// Return the Record
}

string CIniFile::GetValue(string KeyName, string SectionName)
{
	vector<Record> content = GetRecord(KeyName, SectionName);		// Get the Record

	if (!content.empty())													// Make sure there is a value to return
		return content[0].Value;											// And return the value

	return "";																// No value was found
}

bool CIniFile::SetValue(string KeyName, string Value, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		m_bChanged = true;

		if (!SectionExists(SectionName))									// If the Section doesn't exist
		{
			Record s = { "",' ',SectionName,"","" };						// Define a new section
			Record r = { "",' ',SectionName,KeyName,Value };				// Define a new record
			m_vecRec.push_back(s);											// Add the section
			m_vecRec.push_back(r);											// Add the record
			return true;
			//return Save();												// Save
		}

		if (!RecordExists(KeyName, SectionName))							// If the Key doesn't exist
		{
			vector<Record>::iterator iter = std::find_if(m_vecRec.begin(),
				m_vecRec.end(),
				CIniFile::RecordSectionIs(SectionName));					// Locate the Section
			iter++;															// Advance just past the section
			Record r = { "",' ',SectionName,KeyName,Value };				// Define a new record
			m_vecRec.insert(iter, r);										// Add the record
			return true;
			//return Save();												// Save
		}

		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(),
			m_vecRec.end(),
			CIniFile::RecordSectionKeyIs(SectionName, KeyName));			// Locate the Record

		iter->Value = Value;												// Insert the correct value
		return true;
		//return Save();													// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::RenameSection(string OldSectionName, string NewSectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		m_bChanged = true;

		for (vector<Record>::iterator iter = m_vecRec.begin();
			iter < m_vecRec.end(); iter++)									// Loop through the records
		{
			if (iter->Section == OldSectionName)							// Is this the OldSectionName?
				iter->Section = NewSectionName;								// Now its the NewSectionName
		}
		return true;
		//return Save();													// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::CommentRecord(CommentChar cc, string KeyName, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(),
			m_vecRec.end(),
			CIniFile::RecordSectionKeyIs(SectionName, KeyName));			// Locate the Section/Key

		if (iter == m_vecRec.end()) return false;							// The Section/Key was not found

		iter->Commented = cc;												// Change the Comment value
		m_bChanged = true;
		return true;
		//return Save();													// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::UnCommentRecord(string KeyName, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(), 
			m_vecRec.end(),
				CIniFile::RecordSectionKeyIs(SectionName,KeyName));			// Locate the Section/Key

		if (iter == m_vecRec.end()) return false;							// The Section/Key was not found
	
		iter->Commented = ' ';												// Remove the Comment value
		m_bChanged = true;
		return true;
		//return Save();													// Save

	}
	return false;															// In the event the file does not load
}

bool CIniFile::CommentSection(char CommentChar, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		for (vector<Record>::iterator iter = m_vecRec.begin(); iter < m_vecRec.end(); iter++)
		{
			if (iter->Section == SectionName)								// Is this the right section?
				iter->Commented = CommentChar;								// Change the comment value
		}
		
		m_bChanged = true;
		return true;
		//return Save();													// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::UnCommentSection(string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		for (vector<Record>::iterator iter = m_vecRec.begin(); iter < m_vecRec.end(); iter++)
		{
			if (iter->Section == SectionName)								// Is this the right section?
				iter->Commented = ' ';										// Remove the comment value
		}

		m_bChanged = true;
		return true;
		//return Save();													// Save
	}

	return false;															// In the event the file does not load
}

bool CIniFile::DeleteRecord(string KeyName, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(), 
			m_vecRec.end(),
				CIniFile::RecordSectionKeyIs(SectionName,KeyName));			// Locate the Section/Key

		if (iter == m_vecRec.end()) return false;							// The Section/Key was not found
	
		m_vecRec.erase(iter);												// Remove the Record

		m_bChanged = true;
		return true;
		//return Save();													// Save

	}
	
	return false;															// In the event the file does not load
}

bool CIniFile::DeleteSection(string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		for (int i = (int)m_vecRec.size() - 1; i>-1; i--)					// Iterate backwards through the content
		{
			if (m_vecRec[i].Section == SectionName)							// Is this related to the Section?
				m_vecRec.erase(m_vecRec.begin() + i);						// Then erase it
		}

		m_bChanged = true;
		return true;
		//return Save();													// Save
	}
	return false;															// In the event the file does not load
}

bool CIniFile::SetSectionComments(string Comments, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		for (vector<Record>::iterator iter = m_vecRec.begin(); iter < m_vecRec.end(); iter++)									// Loop through the records
		{
			if ((iter->Section == SectionName) &&							// Is this the Section?
				(iter->Key == ""))											// And not a record
			{
				if (Comments.size() >= 2)									// Is there a comment?
				{
					if (Comments.substr(Comments.size() - 2) != "\n")		// Does the string end in a newline?
						Comments += "\n";									// If not, add one
				}
				iter->Comments = Comments;									// Set the comments

				m_bChanged = true;
				return true;
				//return Save();											// Save
			}
		}
	}
	return false;															// In the event the file does not load
}

bool CIniFile::SetRecordComments(string Comments, string KeyName, string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		vector<Record>::iterator iter = std::find_if(m_vecRec.begin(),
			m_vecRec.end(),
			CIniFile::RecordSectionKeyIs(SectionName, KeyName));			// Locate the Section/Key

		if (iter == m_vecRec.end()) return false;							// The Section/Key was not found

		if (Comments.size() >= 2)											// Is there a comment?
		{
			if (Comments.substr(Comments.size() - 2) != "\n")					// Does the string end in a newline?
				Comments += "\n";											// If not, add one
		}
		iter->Comments = Comments;											// Set the comments

		m_bChanged = true;
		return true;
		//return Save();													// Save
	}

	return false;															// In the event the file does not load
}

vector<CIniFile::Record> CIniFile::GetSections()
{
	vector<Record> data;													// Holds the return data

	if (m_bLoaded)															// Make sure the file is loaded
	{
		for (int i = 0; i<(int)m_vecRec.size(); i++)						// Loop through the content
		{
			if (m_vecRec[i].Key == "")										// If this is a section 
				data.push_back(m_vecRec[i]);								// Add the record to the return data
		}
	}

	return data;															// Return the data
}

bool CIniFile::Sort(bool Descending)
{
	vector<CIniFile::Record> sections = GetSections();						// Get a list of Sections

	if (!sections.empty())													// Is there anything to process?
	{

		if (Descending)														// Descending or Ascending?
			std::sort(sections.begin(), sections.end(), DescendingSectionSort());
		else																// Sort the Sections
			std::sort(sections.begin(), sections.end(), AscendingSectionSort());

		for (vector<Record>::iterator iter = sections.begin(); iter < sections.end(); iter++) // For each Section
		{
			m_vecRec.push_back(*iter);										// Add the sorted Section to the content

			vector<CIniFile::Record> records = GetSection(iter->Section); // Get a list of Records for this section

			if (Descending)													// Descending or Ascending?
				std::sort(records.begin(), records.end(), DescendingRecordSort());
			else															// Sort the Records
				std::sort(records.begin(), records.end(), AscendingRecordSort());

			for (vector<Record>::iterator it = records.begin(); it < records.end(); it++) // For each Record
				m_vecRec.push_back(*it);									// Add the sorted Record to the content
		}

		m_bChanged = true;
		return true;
		//return Save();													// Save
	}

	return false;															// There were no sections
}

bool CIniFile::AddSection(string SectionName)
{
	if (m_bLoaded)															// Make sure the file is loaded
	{
		if (!SectionExists(SectionName))
		{
			Record s = { "",' ',SectionName,"","" };						// Define a new section
			m_vecRec.push_back(s);											// Add the section

			m_bChanged = true;
			return true;
			//return Save();												// Save
		}

		return true;
	}

	return false;															// The file did not open
}

bool CIniFile::Create()
{
	return Save();															// Save
}


void CIniFile::StringToChar(CString str, char* pCh) // char* returned must be deleted... 
{
	wchar_t*	wszStr;
	int				nLenth;

	USES_CONVERSION;
	//1. CString to wchar_t* conversion
	wszStr = T2W(str.GetBuffer(str.GetLength()));

	//2. wchar_t* to char* conversion
	nLenth = WideCharToMultiByte(CP_ACP, 0, wszStr, -1, NULL, 0, NULL, NULL); //char* 형에 대한길이를 구함 

																			  //3. wchar_t* to char* conversion
	WideCharToMultiByte(CP_ACP, 0, wszStr, -1, pCh, nLenth, 0, 0);
	return;
}

CString CIniFile::CharToString(const char *szStr)
{
	CString strRet;

	int nLen = strlen(szStr) + sizeof(char);
	wchar_t *tszTemp = NULL;
	tszTemp = new WCHAR[nLen];

	MultiByteToWideChar(CP_ACP, 0, szStr, -1, tszTemp, nLen * sizeof(WCHAR));

	strRet.Format(_T("%s"), (CString)tszTemp);
	if (tszTemp)
	{
		delete[] tszTemp;
		tszTemp = NULL;
	}
	return strRet;
}
