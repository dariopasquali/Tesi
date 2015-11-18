#include "IntraCategoryTester.h"
#include "commonInclude.h"



using namespace std;
using namespace cv;
using namespace od;

IntraCategoryTester::IntraCategoryTester(od::ObjectDetector* detector) :
			CategoryTester(detector)
{}

IntraCategoryTester::IntraCategoryTester(od::ObjectDetector* detector, char* datasetDirectory) : 
			CategoryTester(detector, datasetDirectory)
{}

IntraCategoryTester::IntraCategoryTester(od::ObjectDetector* detector, double hammingThreshold, double correlationThreshold) :
			CategoryTester(detector, hammingThreshold, correlationThreshold)
{}

IntraCategoryTester::IntraCategoryTester(od::ObjectDetector* detector, char* datasetDirectory, double hammingThreshold, double correlationThreshold) :
			CategoryTester(detector, datasetDirectory, hammingThreshold, correlationThreshold)
{}

double IntraCategoryTester::categoryDetectionRate()
{
	TCHAR setDir[MAX_PATH];
	WIN32_FIND_DATA file;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	StringCchCopy(setDir, MAX_PATH, _datasetDirectory);
	StringCchCat(setDir, MAX_PATH, TEXT("\\"));
	StringCchCat(setDir, MAX_PATH, _choosenCategory);
	StringCchCat(setDir, MAX_PATH, TEXT("\\*"));

	cout << "====================================================" << endl;
	cout << "INTRA CATEGORY TEST" << endl << "TEST IS STARTING" << endl;
	cout << "====================================================" << endl;

	hFind = INVALID_HANDLE_VALUE;

	/*
		Per ogni file della categoria
			Ne salvo il path
			Lo carico nel detector
			
			Per ogni altro file della categoria
				Se non è quello sample
					confrontalo col sample
					memorizza se l'ha trovato o meno


			calcola la percentuale di oggetti trovati
			salvala associata al file sample

		Calcola la media delle percentuali di oggetti trovati per tutti i file
		restituisci il valore	
	*/

	vector<char*> categoryFiles;
	
	double totDetected = 0;
	double totCompared = 0;

	//vector<bool> detected;
	//map<int, double> tableOfDetectionPercentage;

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

			//TCHAR fileName[MAX_PATH];
			//StringCchCopy(fileName, MAX_PATH, _choosenCategory);
			//StringCchCat(fileName, MAX_PATH, TEXT("\\"));
			//StringCchCat(fileName, MAX_PATH, file.cFileName);

			categoryFiles.push_back(fileName);
		}
	
	} while (FindNextFile(hFind, &file) != 0);
	FindClose(hFind);

	cout << "filenames loaded" << endl;

	for (int i = 0; i < categoryFiles.size(); i++)
	{
		//detected.clear();

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

		for (int k = 0; k < categoryFiles.size(); k++)
		{
			if (k != i)
			{
				queryImage = imread(categoryFiles[k]);
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
					totDetected++;
					cout << "Detected :)" << endl;
				}
				else
				{
					cout << "not Detected :(" << endl;
				}
				totCompared++;
			}			
		}
		
	}

	cout << "Calculate Category Detection Rate" << endl;

	double CDR = 0;

	CDR = totDetected / totCompared;
	CDR *= 100;

	return CDR;
}