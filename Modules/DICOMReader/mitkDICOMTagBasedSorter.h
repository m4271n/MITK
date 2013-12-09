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

#ifndef mitkDICOMTagBasedSorter_h
#define mitkDICOMTagBasedSorter_h

#include "mitkDICOMDatasetSorter.h"
#include "mitkDICOMSortCriterion.h"

namespace mitk
{

/**
  \brief sort files based on filename (last resort).
*/
class DICOMReader_EXPORT DICOMTagBasedSorter : public DICOMDatasetSorter
{
  public:

    mitkClassMacro( DICOMTagBasedSorter, DICOMDatasetSorter )
    itkNewMacro( DICOMTagBasedSorter )

    void AddDistinguishingTag( const DICOMTag& );
    void SetSortCriterion( DICOMSortCriterion::ConstPointer criterion );

    virtual DICOMTagList GetTagsOfInterest();

    virtual void Sort();

  protected:

    struct ParameterizedDatasetSort
    {
      ParameterizedDatasetSort(DICOMSortCriterion::ConstPointer);
      bool operator() (const mitk::DICOMDatasetAccess* left, const mitk::DICOMDatasetAccess* right);
      bool StringCompare(const mitk::DICOMDatasetAccess* left, const mitk::DICOMDatasetAccess* right);
      bool NumericCompare(const mitk::DICOMDatasetAccess* left, const mitk::DICOMDatasetAccess* right);
      DICOMSortCriterion::ConstPointer m_SortCriterion;
    };


    DICOMTagBasedSorter();
    virtual ~DICOMTagBasedSorter();

    DICOMTagBasedSorter(const DICOMTagBasedSorter& other);
    DICOMTagBasedSorter& operator=(const DICOMTagBasedSorter& other);

    std::string BuildGroupID( DICOMDatasetAccess* dataset );

    typedef std::map<std::string, DICOMDatasetList> GroupIDToListType;
    GroupIDToListType SplitInputGroups();
    GroupIDToListType& SortGroups(GroupIDToListType& groups);

    DICOMTagList m_DistinguishingTags;
    DICOMSortCriterion::ConstPointer m_SortCriterion;
};

}

#endif
