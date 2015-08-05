/*===================================================================

The Medical Imaging Interaction Toolkit (MITK)

Copyright (c) German Cancer Research Center,
Division of Medical and Biological Informatics.
All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR
A PARTICULAR PURPOSE.

See LICENSE.txt or http://www.mitk.org for details.

===================================================================*/

#include <mitkTestingMacros.h>
#include <mitkTestFixture.h>
#include "mitkIOUtil.h"
#include "itkArray2D.h"

#include <itkCSVArray2DFileReader.h>
#include <itkCSVArray2DDataObject.h>
#include <mitkVigraRandomForestClassifier.h>
#include <itkLabelSampler.h>
#include <itkAddImageFilter.h>
#include <mitkImageCast.h>
#include <mitkStandaloneDataStorage.h>

class mitkVigraRandomForestTestSuite : public mitk::TestFixture
{
  CPPUNIT_TEST_SUITE(mitkVigraRandomForestTestSuite  );
  MITK_TEST(TrainThreadedDecisionForest_MatlabDataSet_shouldReturnTrue);
  MITK_TEST(TrainThreadedDecisionForest_BreastCancerDataSet_shouldReturnTrue);
  CPPUNIT_TEST_SUITE_END();

private:

  typedef Eigen::Matrix<double ,Eigen::Dynamic,Eigen::Dynamic> MatrixDoubleType;
  typedef Eigen::Matrix<int, Eigen::Dynamic,Eigen::Dynamic> MatrixIntType;

  Eigen::MatrixXd m_TrainingMatrixX;
  Eigen::MatrixXi m_TrainingLabelMatrixY;
  Eigen::MatrixXd m_TestXPredict;
  Eigen::MatrixXi m_TestYPredict;

  mitk::VigraRandomForestClassifier::Pointer classifier;

public:

  /*Reading an file, which includes the trainingdataset and the testdataset, and convert the
  content of the file into an 2dim matrixpair.
  There are an delimiter, which separates the matrix into an trainingmatrix and testmatrix */
  template<typename T>
  std::pair<Eigen::Matrix<T ,Eigen::Dynamic,Eigen::Dynamic>,Eigen::Matrix<T ,Eigen::Dynamic,Eigen::Dynamic> >convertCSVToMatrix(const std::string &path, char delimiter,double range, bool isXMatrix)
  {
    typename itk::CSVArray2DFileReader<T>::Pointer fr = itk::CSVArray2DFileReader<T>::New();
    fr->SetFileName(path);
    fr->SetFieldDelimiterCharacter(delimiter);
    fr->HasColumnHeadersOff();
    fr->HasRowHeadersOff();
    fr->Parse();
    try{
      fr->Update();
    }catch(itk::ExceptionObject& ex){
      cout << "Exception caught!" << std::endl;
      cout << ex << std::endl;
    }

    typename itk::CSVArray2DDataObject<T>::Pointer p = fr->GetOutput();
    unsigned int maxrowrange = p->GetMatrix().rows();
    unsigned int c = p->GetMatrix().cols();
    unsigned int percentRange = (unsigned int)(maxrowrange*range);

    if(isXMatrix == true){
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> trainMatrixX(percentRange,c);
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> testMatrixXPredict(maxrowrange-percentRange,c);

      for(int row = 0; row < percentRange; row++){
        for(int col = 0; col < c; col++){
          trainMatrixX(row,col) =  p->GetData(row,col);
        }
      }

      for(int row = percentRange; row < maxrowrange; row++){
        for(int col = 0; col < c; col++){
          testMatrixXPredict(row-percentRange,col) =  p->GetData(row,col);
        }
      }

      return std::make_pair(trainMatrixX,testMatrixXPredict);
    }
    else if(isXMatrix == false){
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> trainLabelMatrixY(percentRange,c);
      Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> testMatrixYPredict(maxrowrange-percentRange,c);

      for(int row = 0; row < percentRange; row++){
        for(int col = 0; col < c; col++){
          trainLabelMatrixY(row,col) =  p->GetData(row,col);
        }
      }

      for(int row = percentRange; row < maxrowrange; row++){
        for(int col = 0; col < c; col++){
          testMatrixYPredict(row-percentRange,col) =  p->GetData(row,col);
        }
      }

      return std::make_pair(trainLabelMatrixY,testMatrixYPredict);
    }
  }

  /*
  Reading an csv-data and transfer the included datas into an matrix.
  */
  template<typename T>
  Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> readCsvData(const std::string &path, char delimiter)
  {
    typename itk::CSVArray2DFileReader<T>::Pointer fr = itk::CSVArray2DFileReader<T>::New();
    fr->SetFileName(path);
    fr->SetFieldDelimiterCharacter(delimiter);
    fr->HasColumnHeadersOff();
    fr->HasRowHeadersOff();
    fr->Parse();
    try{
      fr->Update();
    }catch(itk::ExceptionObject& ex){
      cout << "Exception caught!" << std::endl;
      cout << ex << std::endl;
    }

    typename itk::CSVArray2DDataObject<T>::Pointer p = fr->GetOutput();
    unsigned int maxrowrange = p->GetMatrix().rows();
    unsigned int maxcols = p->GetMatrix().cols();
    Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> matrix(maxrowrange,maxcols);

    for(int rows = 0; rows < maxrowrange; rows++){
      for(int cols = 0; cols < maxcols; cols++ ){
        matrix(rows,cols) = p->GetData(rows,cols);
      }
    }

    return matrix;
  }

  /*
  Write the content of the array into an own csv-data in the following sequence:
  root.csv:   1    2    3    0   0    4
  writen.csv:     1  1:2  2:3  3:0 4:0  5:4
  */
  template<typename T>
  void writeMatrixToCsv(Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> paramMatrix,const std::string &path)
  {
    std::ofstream outputstream (path,std::ofstream::out);   // 682

    if(outputstream.is_open()){
      for(int i = 0; i < paramMatrix.rows(); i++){
        outputstream << paramMatrix(i,0);
        for(int j = 1; j < 11; j++){
          outputstream  << " " << j << ":" << paramMatrix(i,j);
        }
        outputstream << endl;
      }
      outputstream.close();
    }
    else{
      cout << "Unable to write into CSV" << endl;
    }
  }

  /*
  Train the classifier with an exampledataset of mattlab.
  Note: The included data are gau�an normaldistributed.
  */
  void TrainThreadedDecisionForest_MatlabDataSet_shouldReturnTrue()
  {
    /* Declarating an featurematrixdataset, the first matrix
    of the matrixpair is the trainingmatrix and the second one is the testmatrix.*/
    std::pair<MatrixDoubleType,MatrixDoubleType> matrixDouble;
    matrixDouble = convertCSVToMatrix<double>(GetTestDataFilePath("Classification/FeaturematrixMatlab.csv"),';',0.5,true);
    m_TrainingMatrixX = matrixDouble.first;
    m_TestXPredict = matrixDouble.second;

    /* The declaration of the labelmatrixdataset is equivalent to the declaration
    of the featurematrixdataset.*/

    std::pair<MatrixIntType,MatrixIntType> matrixInt;
    matrixInt = convertCSVToMatrix<int>(GetTestDataFilePath("Classification/LabelmatrixMatlab.csv"),';',0.5,false);
    m_TrainingLabelMatrixY = matrixInt.first;
    m_TestYPredict = matrixInt.second;
    classifier = mitk::VigraRandomForestClassifier::New();

    /* Train the classifier, by giving trainingdataset for the labels and features.
    The result in an colunmvector of the labels.*/
    classifier->Train(m_TrainingMatrixX,m_TrainingLabelMatrixY);
    Eigen::MatrixXi classes = classifier->Predict(m_TestXPredict);

    /* Testing the matching between the calculated colunmvector and the result of the RandomForest */
    unsigned int maxrows = classes.rows();

    bool isYPredictVector = false;
    int count = 0;

    for(int i= 0; i < maxrows; i++){
      if(classes(i,0) == m_TestYPredict(i,0)){
        isYPredictVector = true;
        count++;
      }
    }
    MITK_INFO << 100*count/(double)(maxrows) << "%";
    MITK_TEST_CONDITION(isIntervall<int>(m_TestYPredict,classes,97,99),"Testvalue is in range.");
  }

  // Method for intervalltesting
  template<typename T>
  bool isIntervall(Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> expected, Eigen::Matrix<T,Eigen::Dynamic,Eigen::Dynamic> actual, double lowrange, double toprange)
  {
    bool isInIntervall = false;
    int count = 0;
    unsigned int rowRange = expected.rows();
    unsigned int colRange = expected.cols();
    for(int i = 0; i < rowRange; i++){
      for(int j = 0; j < colRange; j++){
        if(expected(i,j) == actual(i,j)){
          count++;
        }
      }

      double valueOfMatch = 100*count/(double)(rowRange);
      if((lowrange <= valueOfMatch) && (toprange >= valueOfMatch)){
        isInIntervall = true;
      }
    }
    return isInIntervall;
  }

  /*
  Train the classifier with the dataset of breastcancer patients from the
  LibSVM Libary
  */
  void TrainThreadedDecisionForest_BreastCancerDataSet_shouldReturnTrue()
  {
    /* Declarating an featurematrixdataset, the first matrix
    of the matrixpair is the trainingmatrix and the second one is the testmatrix.*/
    std::pair<MatrixDoubleType,MatrixDoubleType> matrixDouble;
    matrixDouble = convertCSVToMatrix<double>(GetTestDataFilePath("Classification/FeaturematrixBreastcancer.csv"),';',0.5,true);
    m_TrainingMatrixX = matrixDouble.first;
    m_TestXPredict = matrixDouble.second;

    /* The declaration of the labelmatrixdataset is equivalent to the declaration
    of the featurematrixdataset.*/
    std::pair<MatrixIntType,MatrixIntType> matrixInt;
    matrixInt = convertCSVToMatrix<int>(GetTestDataFilePath("Classification/LabelmatrixBreastcancer.csv"),';',0.5,false);
    m_TrainingLabelMatrixY = matrixInt.first;
    m_TestYPredict = matrixInt.second;

    classifier = mitk::VigraRandomForestClassifier::New();

    /* Train the classifier, by giving trainingdataset for the labels and features.
    The result in an colunmvector of the labels.*/
    classifier->Train(m_TrainingMatrixX,m_TrainingLabelMatrixY);
    Eigen::MatrixXi classes = classifier->Predict(m_TestXPredict);

    /* Testing the matching between the calculated colunmvector and the result of the RandomForest */
    unsigned int maxrows = classes.rows();

    bool isYPredictVector = false;
    int count = 0;

    for(int i= 0; i < maxrows; i++){
      if(classes(i,0) == m_TestYPredict(i,0)){
        isYPredictVector = true;
        count++;
      }
    }
    MITK_INFO << 100*count/(double)(maxrows) << "%";
    MITK_TEST_CONDITION(isIntervall<int>(m_TestYPredict,classes,97,99),"Testvalue is in range.");
  }

  void TestThreadedDecisionForest()
  {
  }
};

MITK_TEST_SUITE_REGISTRATION(mitkVigraRandomForest)