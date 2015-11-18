#include "InterCategoryTester.h"
#include "commonInclude.h"

using namespace std;
using namespace cv;
using namespace od;

InterCategoryTester::InterCategoryTester(od::ObjectDetector* detector) :
CategoryTester(detector)
{}

InterCategoryTester::InterCategoryTester(od::ObjectDetector* detector, char* datasetDirectory) :
CategoryTester(detector, datasetDirectory)
{}

InterCategoryTester::InterCategoryTester(od::ObjectDetector* detector, double hammingThreshold, double correlationThreshold) :
CategoryTester(detector, hammingThreshold, correlationThreshold)
{}

InterCategoryTester::InterCategoryTester(od::ObjectDetector* detector, char* datasetDirectory, double hammingThreshold, double correlationThreshold) :
CategoryTester(detector, datasetDirectory, hammingThreshold, correlationThreshold)
{}

double InterCategoryTester::categoryDetectionRate()
{
	TCHAR setDir[MAX_PATH];
	WIN32_FIND_DATA file;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	TCHAR subSetDir[MAX_PATH];
	WIN32_FIND_DATA subFile;
	HANDLE subHFind = INVALID_HANDLE_VALUE;

	StringCchCopy(setDir, MAX_PATH, _datasetDirectory);
	StringCchCat(setDir, MAX_PATH, TEXT("\\"));
	StringCchCat(setDir, MAX_PATH, _choosenCategory);
	StringCchCat(setDir, MAX_PATH, TEXT("\\*"));

	cout << "====================================================" << endl;
	cout << "INTER CATEGORY TEST" << endl << "TEST IS STARTING" << endl;
	cout << "====================================================" << endl;

	hFind = INVALID_HANDLE_VALUE;

	/*
	Per ogni file della categoria1
		Lo carico nel detector

		Per ogni altra categoria2
			
			Per ogni altro file della categoria2
				confrontalo col sample
				memorizza se l'ha trovato o meno				
				incrementa il numero dei confronti con il sample

		calcola la percentuale di oggetti trovati
		salvala associata al file sample

	Calcola la media delle percentuali di oggetti trovati per tutti i file della categoria 1
	restituisci il valore
	*/

	vector<char*> categoryFiles;
	vector<char*> compareFiles;

	//vector<bool> detected;
	//map<int, bool> tableOfDetectionPercentage;

	double totCompared = 0;
	double totNotDetected = 0;

	double categoryDetectionRate = 0;

	Mat sampleImage;
	Mat queryImage;

	hFind = FindFirstFile(setDir, &file);

	namedWindow("SAMPLE", 1);
	

	do
	{
		if (file.dwFileAttributes &&
			strcmp(file.cFileName, ".") != 0 &&
			strcmp(file.cFileName, "..") != 0 &&
			strcmp(file.cFileName, "Thumbs.db") != 0)
		{
			char* fileName = new char[MAX_PATH];
			strcpy(fileName, _datasetDirectory);
			strcat(fileName, "\\");
			strcat(fileName, _choosenCategory);
			strcat(fileName, "\\");
			strcat(fileName, file.cFileName);

			categoryFiles.push_back(fileName);
		}

	} while (FindNextFile(hFind, &file) != 0);
	FindClose(hFind);

	cout << "filenames loaded" << endl;

	for (int i = 0; i < categoryFiles.size(); i++) // per ogni file sample
	{

		sampleImage = imread(categoryFiles[i]);
		if (sampleImage.empty())
		{
			cout << "---> ERROR: CAN'T READ SAMPLE IMAGE <---" << endl;
			cout << "---> STOPPING TEST <---" << endl;
			return 0;
		}

		if (!_detector->loadImage(sampleImage))
		{
			cout << "---> ERROR: CAN'T LOAD SAMPLE IMAGE IN THE DETECTOR <---" << endl;
			cout << "---> STOPPING TEST <---" << endl;
			return 0;
		}

		cout << "Sample image loaded in the detector" << endl;
		imshow("SAMPLE", sampleImage);
		waitKey(33);

		for (int j = 0; j < _categories.size(); j++) 
		{
			if (strcmp(_categories[j].c_str(), _choosenCategory) != 0)  // Per ogni altra categoria
			{
				StringCchCopy(subSetDir, MAX_PATH, _datasetDirectory);
				StringCchCat(subSetDir, MAX_PATH, TEXT("\\"));
				StringCchCat(subSetDir, MAX_PATH, _categories[j].c_str());
				StringCchCat(subSetDir, MAX_PATH, TEXT("\\*"));

				subHFind = FindFirstFile(subSetDir, &subFile);
				
				compareFiles.clear();

				do
				{
					if (subFile.dwFileAttributes &&
						strcmp(subFile.cFileName, ".") != 0 &&
						strcmp(subFile.cFileName, "..") != 0 &&
						strcmp(subFile.cFileName, "Thumbs.db") != 0)
					{
						char* fileName = new char[MAX_PATH];
						strcpy(fileName, _datasetDirectory);
						strcat(fileName, "\\");
						strcat(fileName, _categories[j].c_str());
						strcat(fileName, "\\");
						strcat(fileName, subFile.cFileName);

						compareFiles.push_back(fileName);
					}

				} while (FindNextFile(subHFind, &subFile) != 0);


				for (int k = 0; k < compareFiles.size(); k++) // Per ogni file dell'altra categoria
				{
					queryImage = imread(compareFiles[k]);
					if (queryImage.empty())
					{
						cout << "---> ERROR: CAN'T READ QUERY IMAGE <---" << endl;
						cout << "---> STOPPING TEST <---" << endl;
						return 0;
					}

					cout << "Load query image " << to_string(k) << endl;

					int numberOfObjects = 0;
					vector<vector<vector<Point>>> objects;

					_detector->findObjectsInImage(queryImage,
						_hammingThreshold, _correlationThreshold,
						ObjectDetector::OutputMaskMode::NO_MASK,
						&objects, &numberOfObjects);

					if (numberOfObjects > 0)
					{
						
						cout << "Detected :)" << endl;
					}
					else
					{
						totNotDetected++;
						cout << "not Detected :(" << endl;
					}
					totCompared++;
				}
			}			
		}

	}

	cout << "Calculate Category Detection Rate" << endl;

	double totalCDR = 0;

	totalCDR = totNotDetected / totCompared;
	totalCDR *= 100;

	return totalCDR;



}