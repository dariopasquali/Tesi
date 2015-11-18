#include "CategoryTester.h"
#include "commonInclude.h"



using namespace std;
using namespace cv;
using namespace od;

CategoryTester::CategoryTester(od::ObjectDetector* detector) :
			_detector(detector),
			_datasetDirectory("101_test"),
			_hammingThreshold(70),
			_correlationThreshold(40),
			_choosenCategory(NULL)
{}

CategoryTester::CategoryTester(od::ObjectDetector* detector, char* datasetDirectory) :
			_detector(detector),
			_datasetDirectory(datasetDirectory),
			_hammingThreshold(70),
			_correlationThreshold(40),
			_choosenCategory(NULL)
{}

CategoryTester::CategoryTester(od::ObjectDetector* detector, double hammingThreshold, double correlationThreshold) :
			_detector(detector),
			_datasetDirectory("101_test"),
			_hammingThreshold(hammingThreshold),
			_correlationThreshold(correlationThreshold),
			_choosenCategory(NULL)
{}

CategoryTester::CategoryTester(od::ObjectDetector* detector, char* datasetDirectory, double hammingThreshold, double correlationThreshold) :
			_detector(detector),
			_datasetDirectory(datasetDirectory),
			_hammingThreshold(hammingThreshold),
			_correlationThreshold(correlationThreshold),
			_choosenCategory(NULL)
{}

std::vector<std::string> CategoryTester::loadCategories()
{
	_categories.clear();

	TCHAR szDir[MAX_PATH];
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	StringCchCopy(szDir, MAX_PATH, _datasetDirectory);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);

	
	while (FindNextFile(hFind, &ffd) != 0)
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
			strcmp(ffd.cFileName, ".") != 0 &&
			strcmp(ffd.cFileName, "..") != 0 &&
			strcmp(ffd.cFileName, "Thumbs.db") != 0)
		{
			_categories.push_back(ffd.cFileName);
		}
	}
	FindClose(hFind);

	return _categories;
}

bool CategoryTester::setCategory(char* category)
{
	bool find = false;

	for (int i = 0; i < _categories.size() && !find; i++)
	{
		find = (strcmp(category, _categories[i].c_str()) == 0);		
	}

	if (find)
	{
		_choosenCategory = new char[MAX_PATH];
		//StringCchCopy(_choosenCategory, MAX_PATH, _datasetDirectory);
		//StringCchCat(_choosenCategory, MAX_PATH, TEXT("\\"));
		StringCchCopy(_choosenCategory, MAX_PATH, category);
		//StringCchCat(_choosenCategory, MAX_PATH, TEXT("\\*"));
	}

	return find;
}


double CategoryTester::startTest()
{
	if (_datasetDirectory == NULL)
	{
		cout << "---> INVALID DATASET DIRECTORY <---" << endl;
		return 0;
	}

	TCHAR setDir[MAX_PATH];
	WIN32_FIND_DATA file;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	if (_choosenCategory == NULL)
	{
		cout << "i'm choosing a random category from the dataset" << endl;

		srand(time(NULL));
		int dirNum = rand() % 9;

		StringCchCopy(setDir, MAX_PATH, _datasetDirectory);
		StringCchCat(setDir, MAX_PATH, TEXT("\\*"));

		hFind = FindFirstFile(setDir, &file);

		int d = 0;
		while (d < dirNum && FindNextFile(hFind, &file) != 0)
			d++;

		FindNextFile(hFind, &file);
		if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			
			_choosenCategory = new char[MAX_PATH];
			//StringCchCopy(setDir, MAX_PATH, _datasetDirectory);
			//StringCchCat(setDir, MAX_PATH, TEXT("\\"));
			StringCchCopy(setDir, MAX_PATH, file.cFileName);

		}

		FindClose(hFind);
		cout << "your category is " << _choosenCategory << endl;
	}

	return	categoryDetectionRate();
}