/// @file
/// @author David Pilger <dpilger26@gmail.com>
/// [GitHub Repository](https://github.com/dpilger26/NumCpp)
/// @version 1.0
///
/// @section LICENSE
/// Copyright 2018 David Pilger
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this
/// software and associated documentation files(the "Software"), to deal in the Software 
/// without restriction, including without limitation the rights to use, copy, modify, 
/// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
/// permit persons to whom the Software is furnished to do so, subject to the following 
/// conditions :
///
/// The above copyright notice and this permission notice shall be included in all copies 
/// or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
/// PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE 
/// FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR 
/// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
/// DEALINGS IN THE SOFTWARE.
///
/// @section DESCRIPTION
/// A module for basic image processing
///

#pragma once
#include<NumCpp/NdArray.hpp>
#include<NumCpp/Methods.hpp>
#include<NumCpp/Types.hpp>
#include<NumCpp/Utils.hpp>

#include<cmath>
#include<iostream>
#include<limits>
#include<set>
#include<string>
#include<utility>
#include<vector>

namespace NumCpp
{
    //================================================================================
    // Class Description:
    ///						Class for basic image processing
    template<typename dtype>
    class ImageProcessing
    {
    public:
        //================================================================================
        // Class Description:
        ///						Holds the information for a single pixel
        class Pixel
        {
        private:
            //==================================Attributes================================///
            int32	clusterId_;
            uint32	row_;
            uint32	col_;
            dtype	intensity_;

        public:
            //=============================================================================
            // Description:
            ///              defualt constructor needed by containers
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              None
            ///
            Pixel() :
                clusterId_(-1),
                row_(0),
                col_(0),
                intensity_(0)
            {};

            //=============================================================================
            // Description:
            ///              constructor
            /// 
            /// @param              pixel row,
            /// @param              pixel column,
            /// @param              pixel intensity
            /// 
            /// @return
            ///              None
            ///
            Pixel(uint32 inRow, uint32 inCol, dtype inIntensity) :
                clusterId_(-1),
                row_(inRow),
                col_(inCol),
                intensity_(inIntensity)
            {};

            //=============================================================================
            // Description:
            ///              equality operator
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              bool
            ///
            bool operator==(const Pixel& rhs) const
            {
                return row_ == rhs.row_ && col_ == rhs.col_ && intensity_ == rhs.intensity_;
            }

            //=============================================================================
            // Description:
            ///              not equality operator
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              bool
            ///
            bool operator!=(const Pixel& rhs) const
            {
                return !(*this == rhs);
            }

            //=============================================================================
            // Description:
            ///              less than operator for std::sort algorithm and std::set<>;
            ///              NOTE: std::sort sorts in ascending order. Since I want to sort 
            ///              the centroids in descensing order, I am purposefully defining
            ///              this operator backwards!
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              None
            ///
            bool operator<(const Pixel& rhs) const
            {
                if (row_ < rhs.row_)
                {
                    return true;
                }
                else if (row_ == rhs.row_)
                {
                    if (col_ < rhs.col_)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }

            //=============================================================================
            // Description:
            ///              returns the cluster id that this pixel belongs to
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              cluster id
            ///
            int32 clusterId() const
            {
                return clusterId_;
            }

            //=============================================================================
            // Description:
            ///              sets the cluster id that this pixel belongs to
            /// 
            /// @param
            ///              cluster id
            /// 
            /// @return
            ///              None
            ///
            void setClusterId(int32 inClusterId)
            {
                if (inClusterId < 0)
                {
                    throw std::invalid_argument("ERROR: ImageProcessing::Pixel::setClusterId: input cluster id must be greater than or equal to 0.");
                }

                clusterId_ = inClusterId;
            }

            //=============================================================================
            // Description:
            ///              returns the pixel row
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              row
            ///
            uint32 row() const
            {
                return row_;
            }

            //=============================================================================
            // Description:
            ///              returns the pixel column
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              column
            ///
            uint32 col() const
            {
                return col_;
            }

            //=============================================================================
            // Description:
            ///              returns the pixel intensity
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              intensity
            ///
            dtype intensity() const
            {
                return intensity_;
            }

            //=============================================================================
            // Description:
            ///              returns the pixel information as a string
            /// 
            /// @param
            ///              None
            /// 
            /// @return
            ///              std::string
            ///
            std::string str() const
            {
                std::string out = "row = " + Utils<uint32>::num2str(row_) + " col = " + Utils<uint32>::num2str(col_);
                out += " intensity = " + Utils<dtype>::num2str(intensity_) + "\n";
                return out;
            }

            //============================================================================
            /// Method Description: 
            ///						prints the Pixel object to the console
            ///		
            /// @param
            ///				None
            /// @return
            ///				None
            ///
            void print() const
            {
                std::cout << *this;
            }

            //=============================================================================
            // Description:
            ///              osstream operator
            /// 
            /// @param              std::ostream
            /// @param              Pixel
            /// @return
            ///              std::ostream
            ///
            friend std::ostream& operator<<(std::ostream& inStream, const Pixel& inPixel)
            {
                inStream << inPixel.str();
                return inStream;
            }
        };

        //================================================================================
        // Class Description:
        ///						Holds the information for a cluster of pixels
        class Cluster
        {
        public:
            //================================Typedefs===============================
            typedef typename std::vector<Pixel>::const_iterator    const_iterator;

        private:
            //================================Attributes===============================
            uint32              clusterId_;
            std::vector<Pixel>  pixels_;

            uint32              rowMin_;
            uint32              rowMax_;
            uint32              colMin_;
            uint32              colMax_;

            dtype               intensity_;
            dtype               peakPixelIntensity_;

            double              eod_;

        public:
            //=============================================================================
            // Description:
            ///              default constructor needed by containers
            /// 
            /// @param 
            ///              inClusterId
            /// 
            /// @return 
            ///              None
            ///
            Cluster(uint32 inClusterId) :
                clusterId_(inClusterId),
                rowMin_(std::numeric_limits<uint32>::max()), /// largest possible number
                rowMax_(0),
                colMin_(std::numeric_limits<uint32>::max()), /// largest possible number
                colMax_(0),
                intensity_(0),
                peakPixelIntensity_(0),
                eod_(1.0)
            {};

            //=============================================================================
            // Description:
            ///              equality operator
            /// 
            /// @param 
            ///              Cluster
            /// 
            /// @return 
            ///              bool
            ///
            bool operator==(const Cluster& rhs) const
            {
                if (pixels_.size() != rhs.pixels_.size())
                {
                    return false;
                }

                for (uint32 i = 0; i < pixels_.size(); ++i)
                {
                    if (pixels_[i] != rhs.pixels_[i])
                    {
                        return false;
                    }
                }

                return true;
            }

            //=============================================================================
            // Description:
            ///              not equality operator
            /// 
            /// @param 
            ///              Cluster
            /// 
            /// @return 
            ///              bool
            ///
            bool operator!=(const Cluster& rhs) const
            {
                return !(*this == rhs);
            }

            //=============================================================================
            // Description:
            ///              access operator, no bounds checking
            /// 
            /// @param 
            ///              index
            /// 
            /// @return 
            ///              Pixel
            ///
            const Pixel& operator[](uint32 inIndex) const
            {
                return pixels_[inIndex];
            }

            //=============================================================================
            // Description:
            ///              access method with bounds checking
            /// 
            /// @param 
            ///              index
            /// 
            /// @return 
            ///              Pixel
            ///
            const Pixel& at(uint32 inIndex) const
            {
                if (inIndex >= pixels_.size())
                {
                    throw std::invalid_argument("ERROR: ImageProcessing::Cluster::at: index exceeds cluster size.");
                }
                return pixels_[inIndex];
            }

            //=============================================================================
            // Description:
            ///              returns in iterator to the beginning pixel of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              const_iterator
            ///
            const_iterator begin() const
            {
                return pixels_.cbegin();
            }

            //=============================================================================
            // Description:
            ///              returns in iterator to the 1 past the end pixel of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              const_iterator
            ///
            const_iterator end() const
            {
                return pixels_.cend();
            }

            //=============================================================================
            // Description:
            ///              returns the number of pixels in the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              number of pixels in the cluster
            ///
            uint32 size() const
            {
                return static_cast<uint32>(pixels_.size());
            }

            //=============================================================================
            // Description:
            ///              returns the minimum row number of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              minimum row number of the cluster
            ///
            uint32 clusterId() const
            {
                return clusterId_;
            }

            //=============================================================================
            // Description:
            ///              returns the minimum row number of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              minimum row number of the cluster
            ///
            uint32 rowMin() const
            {
                return rowMin_;
            }

            //=============================================================================
            // Description:
            ///              returns the maximum row number of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              maximum row number of the cluster
            ///
            uint32 rowMax() const
            {
                return rowMax_;
            }

            //=============================================================================
            // Description:
            ///              returns the minimum column number of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              minimum column number of the cluster
            ///
            uint32 colMin() const
            {
                return colMin_;
            }

            //=============================================================================
            // Description:
            ///              returns the maximum column number of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              maximum column number of the cluster
            ///
            uint32 colMax() const
            {
                return colMax_;
            }

            //=============================================================================
            // Description:
            ///              returns the number of rows the cluster spans
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              number of rows
            ///
            uint32 height() const
            {
                return rowMax_ - rowMin_ + 1;
            }

            //=============================================================================
            // Description:
            ///              returns the number of columns the cluster spans
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              number of columns
            ///
            uint32 width() const
            {
                return colMax_ - colMin_ + 1;
            }

            //=============================================================================
            // Description:
            ///              returns the summed intensity of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              summed cluster intensity
            ///
            dtype intensity() const
            {
                return intensity_;
            }

            //=============================================================================
            // Description:
            ///              returns the intensity of the peak pixel in the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              peak pixel intensity
            ///
            dtype peakPixelIntensity() const
            {
                return peakPixelIntensity_;
            }

            //=============================================================================
            // Description:
            ///              returns the cluster estimated energy on detector (EOD)
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              eod
            ///
            double eod() const
            {
                return eod_;
            }

            //=============================================================================
            // Description:
            ///              adds a pixel to the cluster
            /// 
            /// @param 
            ///              pixel
            /// 
            /// @return 
            ///              None
            ///
            void addPixel(const Pixel& inPixel)
            {
                pixels_.push_back(inPixel);
                intensity_ += inPixel.intensity();

                // adjust the cluster bounds
                uint32 row = inPixel.row();
                uint32 col = inPixel.col();
                if (row < rowMin_)
                {
                    rowMin_ = row;
                }
                if (row > rowMax_)
                {
                    rowMax_ = row;
                }
                if (col < colMin_)
                {
                    colMin_ = col;
                }
                if (col > colMax_)
                {
                    colMax_ = col;
                }

                // adjust he peak pixel intensity
                if (inPixel.intensity() > peakPixelIntensity_)
                {
                    peakPixelIntensity_ = inPixel.intensity();
                }

                // calculate the energy on detector estimate
                eod_ = static_cast<double>(peakPixelIntensity_) / static_cast<double>(intensity_);
            }

            //=============================================================================
            // Description:
            ///              returns a string representation of the cluster
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              string
            ///
            std::string str() const
            {
                std::string out;
                for (uint32 i = 0; i < size(); ++i)
                {
                    out += "Pixel " + Utils<uint32>::num2str(i) + ":" + pixels_[i].str();
                }

                return out;
            }

            //============================================================================
            /// Method Description: 
            ///						prints the Cluster object to the console
            ///		
            /// @param
            ///				None
            /// @return
            ///				None
            ///
            void print() const
            {
                std::cout << *this;
            }

            //=============================================================================
            // Description:
            ///              osstream operator
            /// 
            /// @param               std::ostream
            /// @param               Cluster 
            /// @return 
            ///              std::ostream
            ///
            friend std::ostream& operator<<(std::ostream& inStream, const Cluster& inCluster)
            {
                inStream << inCluster.str();
                return inStream;
            }
        };

        private:
        //=============================================================================
        // Class Description:
        ///              Clusters exceedance data into contiguous groups
        class ClusterMaker
        {
        private:
            //==================================Attributes=================================
            const NdArray<bool>* const      xcds_;
            const NdArray<dtype>* const     intensities_;
            std::vector<Pixel>              xcdsVec_;

            Shape                           shape_;

            std::vector<Cluster>            clusters_;

            //=============================================================================
            // Description:
            ///              checks that the input row and column have not fallen off of the edge
            /// 
            /// @param              pixel row, 
            /// @param              pixel column
            /// 
            /// @return 
            ///              returns a pixel object clipped to the image boundaries
            ///
            Pixel makePixel(int32 inRow, int32 inCol)
            {
                // Make sure that on the edges after i've added or subtracted 1 from the row and col that 
                // i haven't gone over the edge
                uint32 row = std::min(static_cast<uint32>(std::max<int32>(inRow, 0)), shape_.rows - 1);
                uint32 col = std::min(static_cast<uint32>(std::max<int32>(inCol, 0)), shape_.cols - 1);
                dtype intensity = intensities_->operator()(row, col);

                return Pixel(row, col, intensity);
            }

            //=============================================================================
            // Description:
            ///              finds all of the neighboring pixels to the input pixel 
            /// 
            /// @param               Pixel
            /// @param               std::set<Pixel> neighbors
            /// @return 
            ///              None
            ///
            void findNeighbors(const Pixel& inPixel, std::set<Pixel>& outNeighbors)
            {
                // using a set will auto take care of adding duplicate pixels on the edges

                // the 8 surrounding neighbors
                int32 row = static_cast<int32>(inPixel.row());
                int32 col = static_cast<int32>(inPixel.col());

                outNeighbors.insert(outNeighbors.end(), makePixel(row - 1, col - 1));
                outNeighbors.insert(outNeighbors.end(), makePixel(row - 1, col));
                outNeighbors.insert(outNeighbors.end(), makePixel(row - 1, col + 1));
                outNeighbors.insert(outNeighbors.end(), makePixel(row, col - 1));
                outNeighbors.insert(outNeighbors.end(), makePixel(row, col + 1));
                outNeighbors.insert(outNeighbors.end(), makePixel(row + 1, col - 1));
                outNeighbors.insert(outNeighbors.end(), makePixel(row + 1, col));
                outNeighbors.insert(outNeighbors.end(), makePixel(row + 1, col + 1));
            }

            //=============================================================================
            // Description:
            ///              finds all of the neighboring pixels to the input pixel that are NOT exceedances
            ///  
            /// @param       Pixel
            /// @param       std::vector<Pixel> neighbors
            /// 
            /// @return 
            ///              vector of non exceedance neighboring pixels
            ///
            void findNeighborNotXcds(const Pixel& inPixel, std::vector<Pixel>& outNeighbors)
            {
                std::set<Pixel> neighbors;
                findNeighbors(inPixel, neighbors);

                // check if the neighboring pixels are exceedances and insert into the xcd vector
                for (auto pixelIter = neighbors.begin(); pixelIter != neighbors.end(); ++pixelIter)
                {
                    if (!xcds_->operator()(pixelIter->row(), pixelIter->col()))
                    {
                        outNeighbors.push_back(*pixelIter);
                    }
                }
            }

            //=============================================================================
            // Description:
            ///              finds the pixel index of neighboring pixels
            ///  
            /// @param       Pixel
            /// @param       std::vector<uint32> neigbhors
            /// 
            /// @return 
            ///              vector of neighboring pixel indices
            ///
            void findNeighborXcds(const Pixel& inPixel, std::vector<uint32>& outNeighbors)
            {
                std::set<Pixel> neighbors;
                findNeighbors(inPixel, neighbors);
                std::vector<Pixel> neighborXcds;

                // check if the neighboring pixels are exceedances and insert into the xcd vector
                for (auto pixelIter = neighbors.begin(); pixelIter != neighbors.end(); ++pixelIter)
                {
                    if (xcds_->operator()(pixelIter->row(), pixelIter->col()))
                    {
                        neighborXcds.push_back(*pixelIter);
                    }
                }

                // loop through the neighbors and find the cooresponding index into exceedances_
                for (auto pixelIter = neighborXcds.begin(); pixelIter < neighborXcds.end(); ++pixelIter)
                {
                    auto theExceedanceIter = find(xcdsVec_.begin(), xcdsVec_.end(), *pixelIter);
                    outNeighbors.push_back(static_cast<uint32>(theExceedanceIter - xcdsVec_.begin()));
                }
            }

            //=============================================================================
            // Description:
            ///              workhorse method that performs the clustering algorithm
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              None
            ///
            void runClusterMaker()
            {
                uint32 clusterId = 0;

                for (uint32 xcdIdx = 0; xcdIdx < xcdsVec_.size(); ++xcdIdx)
                {
                    Pixel& currentPixel = xcdsVec_[xcdIdx];

                    // not already visited
                    if (currentPixel.clusterId() == -1)
                    {
                        Cluster newCluster(clusterId);    // a new cluster
                        currentPixel.setClusterId(clusterId);
                        newCluster.addPixel(currentPixel);  // assign pixel to cluster

                        // get the neighbors
                        std::vector<uint32> neighborIds;
                        findNeighborXcds(currentPixel, neighborIds);
                        if (neighborIds.empty())
                        {
                            clusters_.push_back(newCluster);
                            ++clusterId;
                            continue;
                        }

                        // loop through the neighbors
                        for (uint32 neighborsIdx = 0; neighborsIdx < neighborIds.size(); ++neighborsIdx)
                        {
                            Pixel& currentNeighborPixel = xcdsVec_[neighborIds[neighborsIdx]];

                            // go to neighbors
                            std::vector<uint32> newNeighborIds;
                            findNeighborXcds(currentNeighborPixel, newNeighborIds);

                            // loop through the new neighbors and add them to neighbors
                            for (uint32 newNeighborsIdx = 0; newNeighborsIdx < newNeighborIds.size(); ++newNeighborsIdx)
                            {
                                // not already in neighbors
                                if (find(neighborIds.begin(), neighborIds.end(), newNeighborIds[newNeighborsIdx]) == neighborIds.end())
                                {
                                    neighborIds.push_back(newNeighborIds[newNeighborsIdx]);
                                }
                            }

                            // not already assigned to a cluster
                            if (currentNeighborPixel.clusterId() == -1)
                            {
                                currentNeighborPixel.setClusterId(clusterId);
                                newCluster.addPixel(currentNeighborPixel);
                            }
                        }

                        clusters_.push_back(newCluster);
                        ++clusterId;
                    }
                }
            }

            //=============================================================================
            // Description:
            ///              3x3 dialates the clusters
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              None
            ///
            void expandClusters()
            {
                // loop through the clusters 
                for (auto clusterIter = clusters_.begin(); clusterIter < clusters_.end(); ++clusterIter)
                {
                    // loop through the pixels of the cluster 
                    Cluster& theCluster = *clusterIter;
                    uint32 clusterSize = static_cast<uint32>(theCluster.size());
                    for (uint32 iPixel = 0; iPixel < clusterSize; ++iPixel)
                    {
                        const Pixel& thePixel = theCluster[iPixel];
                        std::vector<Pixel> neighborsNotXcds;
                        findNeighborNotXcds(thePixel, neighborsNotXcds);

                        // loop through the neighbors and if they haven't already been added to the cluster, add them
                        for (auto newPixelIter = neighborsNotXcds.begin(); newPixelIter < neighborsNotXcds.end(); ++newPixelIter)
                        {
                            if (find(theCluster.begin(), theCluster.end(), *newPixelIter) == theCluster.end())
                            {
                                theCluster.addPixel(*newPixelIter);
                            }
                        }
                    }
                }
            }

        public:
            //================================Typedefs=====================================
            typedef typename std::vector<Cluster>::const_iterator   const_iterator;

            //=============================================================================
            // Description:
            ///              constructor
            ///  
            /// @param              NdArray<bool>*, pointer to exceedance array
            /// @param              NdArray<dtype>*, pointer to intensity array
            /// @param				border to apply around exceedance pixels post clustering, default 0
            /// 
            /// @return 
            ///              None
            ///
            ClusterMaker(const NdArray<bool>* const inXcdArrayPtr, const NdArray<dtype>* const inIntensityArrayPtr, uint8 inBorderWidth = 0) :
                xcds_(inXcdArrayPtr),
                intensities_(inIntensityArrayPtr)
            {
                if (xcds_->shape() != intensities_->shape())
                {
                    throw std::invalid_argument("ERROR: ImageProcessing::ClusterMaker(): input xcd and intensity arrays must be the same shape.");
                }

                shape_ = xcds_->shape();

                // convert the NdArray of booleans to a vector of exceedances
                for (uint32 row = 0; row < shape_.rows; ++row)
                {
                    for (uint32 col = 0; col < shape_.cols; ++col)
                    {
                        if (xcds_->operator()(row, col))
                        {
                            Pixel thePixel(row, col, intensities_->operator()(row, col));
                            xcdsVec_.push_back(thePixel);
                        }
                    }
                }

                runClusterMaker();

                for (uint8 i = 0; i < inBorderWidth; ++i)
                {
                    expandClusters();
                }
            }

            //=============================================================================
            // Description:
            ///              returns the number of clusters in the frame
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              number of clusters
            ///
            uint32 size()
            {
                return static_cast<uint32>(clusters_.size());
            }

            //=============================================================================
            // Description:
            ///              access operator, no bounds checking
            /// 
            /// @param 
            ///              index
            /// 
            /// @return 
            ///              None
            ///
            const Cluster& operator[](uint32 inIndex) const
            {
                return clusters_[inIndex];
            }

            //=============================================================================
            // Description:
            ///              access method with bounds checking
            /// 
            /// @param 
            ///              index
            /// 
            /// @return 
            ///              None
            ///
            const Cluster& at(uint32 inIndex) const
            {
                if (inIndex >= clusters_.size())
                {
                    throw std::invalid_argument("ERROR: ImageProcessing::ClusterMaker::at: index exceeds cluster size.");
                }
                return clusters_[inIndex];
            }

            //=============================================================================
            // Description:
            ///              returns in iterator to the beginning cluster of the container
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              const_iterator
            ///
            const_iterator begin() const
            {
                return clusters_.cbegin();
            }

            //=============================================================================
            // Description:
            ///              returns in iterator to the 1 past the end cluster of the container
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              const_iterator
            ///
            const_iterator end() const
            {
                return clusters_.cend();
            }
        };

        public:
        //================================================================================
        // Class Description:
        ///						holds the information for a centroid
        class Centroid
        {
            //==================================Attributes================================///
            double          row_;
            double          col_;
            dtype           intensity_;
            double          eod_;

            //=============================================================================
            // Description:
            ///              center of mass algorithm;
            ///              WARNING: if both positive and negative values are present in the cluster,
            ///              it can lead to an undefined COM.
            /// 
            /// @param 
            ///              cluster
            /// 
            /// @return 
            ///              None
            ///
            void centerOfMass(const Cluster& inCluster)
            {
                Shape clusterShape(inCluster.height(), inCluster.width());
                NdArray<dtype> clusterArray(clusterShape);
                clusterArray.zeros();

                uint32 rowMin = inCluster.rowMin();
                uint32 colMin = inCluster.colMin();
                dtype intensity = inCluster.intensity();

                auto iter = inCluster.begin();
                for (; iter < inCluster.end(); ++iter)
                {
                    clusterArray(iter->row() - rowMin, iter->col() - colMin) = iter->intensity();
                }

                // first get the row center
                row_ = 0;
                uint32 row = rowMin;
                for (uint32 rowIdx = 0; rowIdx < clusterShape.rows; ++rowIdx)
                {
                    double rowSum = 0;
                    for (uint32 colIdx = 0; colIdx < clusterShape.cols; ++colIdx)
                    {
                        rowSum += static_cast<double>(clusterArray(rowIdx, colIdx));
                    }
                    row_ += rowSum * static_cast<double>(row++);
                }

                row_ /= static_cast<double>(intensity);

                // then get the column center
                col_ = 0;
                uint32 col = colMin;
                for (uint32 colIdx = 0; colIdx < clusterShape.cols; ++colIdx)
                {
                    double colSum = 0;
                    for (uint32 rowIdx = 0; rowIdx < clusterShape.rows; ++rowIdx)
                    {
                        colSum += static_cast<double>(clusterArray(rowIdx, colIdx));
                    }
                    col_ += colSum * static_cast<double>(col++);
                }

                col_ /= static_cast<double>(intensity);
            }

        public:
            //=============================================================================
            // Description:
            ///              defualt constructor needed by containers
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              None
            ///
            Centroid() :
                row_(0),
                col_(0),
                intensity_(0),
                eod_(0)
            {};

            //=============================================================================
            // Description:
            ///              constructor
            /// 
            /// @param               centroid id,
            /// @param               FP row,
            /// @param               FP column,
            /// @param               centroid intensity
            /// @param               cluster EOD
            /// @param               cluster number of pixels
            /// 
            /// @return 
            ///              None
            ///
            Centroid(const Cluster& inCluster) :
                row_(0),
                col_(0),
                intensity_(inCluster.intensity()),
                eod_(inCluster.eod())
            {
                centerOfMass(inCluster);
            }

            //=============================================================================
            // Description:
            ///              gets the centroid row
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              centroid row
            ///
            double row() const
            {
                return row_;
            }

            //=============================================================================
            // Description:
            ///              gets the centroid col
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              centroid col
            ///
            double col() const
            {
                return col_;
            }

            //=============================================================================
            // Description:
            ///              gets the centroid intensity
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              centroid intensity
            ///
            dtype intensity() const
            {
                return intensity_;
            }

            //=============================================================================
            // Description:
            ///              returns the estimated eod of the centroid
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              star id
            ///
            double eod() const
            {
                return eod_;
            }

            //=============================================================================
            // Description:
            ///              returns the centroid as a string representation
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              string
            ///
            std::string str() const
            {
                std::string out;
                out += "row = " + Utils<double>::num2str(row_) + " col = " + Utils<double>::num2str(col_);
                out += " intensity = " + Utils<dtype>::num2str(intensity_) + " eod = " + Utils<double>::num2str(eod_) + "\n";

                return out;
            }

            //============================================================================
            /// Method Description: 
            ///						prints the Centroid object to the console
            ///		
            /// @param
            ///				None
            /// @return
            ///				None
            ///
            void print() const
            {
                std::cout << *this;
            }

            //=============================================================================
            // Description:
            ///              equality operator
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              bool
            ///
            bool operator==(const Centroid& rhs) const
            {
                return row_ == rhs.row_ && col_ == rhs.col_ && intensity_ == rhs.intensity_ && eod_ == rhs.eod_;
            }

            //=============================================================================
            // Description:
            ///              not equality operator
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              bool
            ///
            bool operator!=(const Centroid& rhs) const
            {
                return !(*this == rhs);
            }

            //=============================================================================
            // Description:
            ///              less than operator for std::sort algorithm;
            ///              NOTE: std::sort sorts in ascending order. Since I want to sort 
            ///              the centroids in descensing order, I am purposefully defining
            ///              this operator backwards!
            /// 
            /// @param 
            ///              None
            /// 
            /// @return 
            ///              None
            ///
            bool operator<(const Centroid& rhs) const
            {
                return intensity_ < rhs.intensity_ ? false : true;
            }

            //=============================================================================
            // Description:
            ///              ostream operator
            ///  
            /// @param              std::ostream
            /// @param              Centroid
            /// @return 
            ///              std::ostream
            ///
            friend std::ostream& operator<<(std::ostream& inStream, const Centroid& inCentriod)
            {
                inStream << inCentriod.str();
                return inStream;
            }
        };

        //============================================================================
        // Method Description: 
        ///						Applies a threshold to an image
        ///		
        /// @param
        ///				NdArray
        ///				threshold value
        /// @return
        ///				NdArray of booleans of pixels that exceeded the threshold
        ///
        static NdArray<bool> applyThreshold(const NdArray<dtype>& inImageArray, dtype inThreshold)
        {
            return std::move(inImageArray > inThreshold);
        }

        //============================================================================
        // Method Description: 
        ///						Center of Mass centroids clusters
        ///		
        /// @param				NdArray
        /// @param				threshold value
        /// @return
        ///				std::vector<Centroid>
        ///
        static std::vector<Centroid> centroidClusters(const std::vector<Cluster>& inClusters)
        {
            std::vector<Centroid> centroids(inClusters.size());

            for (uint32 i = 0; i < inClusters.size(); ++i)
            {
                centroids[i] = std::move(Centroid(inClusters[i]));
            }

            return std::move(centroids);
        }

        //============================================================================
        // Method Description: 
        ///						Clusters exceedance pixels from an image
        ///		
        /// @param				NdArray
        /// @param				NdArray of exceedances
        /// @param				border to apply around exceedance pixels post clustering, default 0
        /// @return
        ///				std::vector<Cluster>
        ///
        static std::vector<Cluster> clusterPixels(const NdArray<dtype>& inImageArray, const NdArray<bool>& inExceedances, uint8 inBorderWidth = 0)
        {
            ClusterMaker clusterMaker(&inExceedances, &inImageArray, inBorderWidth);
            return std::move(std::vector<Cluster>(clusterMaker.begin(), clusterMaker.end()));
        }

        //============================================================================
        // Method Description: 
        ///						Generates a list of centroids givin an input exceedance
        ///						rate
        ///		
        /// @param				NdArray
        /// @param				exceedance rate
        /// @param              string "pre", or "post" for where to apply the exceedance windowing
        /// @param				border to apply, default 0
        /// @return
        ///				std::vector<Centroid>
        ///
        static std::vector<Centroid> generateCentroids(const NdArray<dtype>& inImageArray, double inRate, const std::string inWindowType, uint8 inBorderWidth = 0)
        {
            uint8 borderWidthPre = 0;
            uint8 borderWidthPost = 0;
            if (inWindowType.compare("pre") == 0)
            {
                borderWidthPre = inBorderWidth;
            }
            else if (inWindowType.compare("post") == 0)
            {
                borderWidthPost = inBorderWidth;
            }
            else
            {
                throw std::invalid_argument("ERROR ImageProcessing::generateCentroids: input window type options are ['pre', 'post']");
            }

            // generate the threshold
            dtype threshold = generateThreshold(inImageArray, inRate);

            // apply the threshold to get xcds
            NdArray<bool> xcds = applyThreshold(inImageArray, threshold);

            // window around the xcds
            if (borderWidthPre > 0)
            {
                xcds = windowExceedances(xcds, borderWidthPre);
            }

            // cluster the exceedances
            std::vector<Cluster> clusters = clusterPixels(inImageArray, xcds, borderWidthPost);

            // centroid the clusters
            return std::move(centroidClusters(clusters));
        }

        //============================================================================
        // Method Description: 
        ///						Calculates a threshold such that the input rate of pixels
        ///						exceeds the threshold. Really should only be used for integer
        ///                      input array values. If using floating point data, user beware...
        ///		
        /// @param				NdArray
        /// @param				exceedance rate
        /// @return
        ///				dtype
        ///
        static dtype generateThreshold(const NdArray<dtype>& inImageArray, double inRate)
        {
            if (inRate < 0 || inRate > 1)
            {
                throw std::invalid_argument("ERROR: ImageProcessing::generateThreshold: input rate must be of the range [0, 1]");
            }

            // first build a histogram
            int32 minValue = static_cast<int32>(std::floor(inImageArray.min().item()));
            int32 maxValue = static_cast<int32>(std::floor(inImageArray.max().item()));

            if (inRate == 0)
            {
                return static_cast<dtype>(maxValue);
            }
            else if (inRate == 1)
            {
                if (DtypeInfo<dtype>::isSigned())
                {
                    return static_cast<dtype>(minValue - 1);
                }
                else
                {
                    return static_cast<dtype>(0);
                }
            }

            uint32 histSize = static_cast<uint32>(maxValue - minValue + 1);

            NdArray<double> histogram(1, histSize);
            histogram.zeros();
            uint32 numPixels = inImageArray.size();
            for (uint32 i = 0; i < numPixels; ++i)
            {
                uint32 bin = static_cast<uint32>(static_cast<int32>(std::floor(inImageArray[i])) - minValue);
                ++histogram[bin];
            }

            // integrate the normalized histogram from right to left to make a survival function (1 - CDF)
            double dNumPixels = static_cast<double>(numPixels);
            NdArray<double> survivalFunction(1, histSize + 1);
            survivalFunction[-1] = 0;
            for (int32 i = histSize - 1; i > -1; --i)
            {
                double histValue = histogram[i] / dNumPixels;
                survivalFunction[i] = survivalFunction[i + 1] + histValue;
            }

            // binary search through the survival function to find the rate
            uint32 indexLow = 0;
            uint32 indexHigh = histSize - 1;
            uint32 index = indexHigh / 2; // integer division

            bool keepGoing = true;
            while (keepGoing)
            {
                double value = survivalFunction[index];
                if (value < inRate)
                {
                    indexHigh = index;
                }
                else if (value > inRate)
                {
                    indexLow = index;
                }
                else
                {
                    int32 thresh = static_cast<int32>(index) + minValue - 1;
                    if (DtypeInfo<dtype>::isSigned())
                    {
                        return static_cast<dtype>(thresh);
                    }
                    else
                    {
                        return thresh < 0 ? 0 : static_cast<dtype>(thresh);
                    }
                }

                if (indexHigh - indexLow < 2)
                {
                    return static_cast<dtype>(static_cast<int32>(indexHigh) + minValue - 1);
                }

                index = indexLow + (indexHigh - indexLow) / 2;
            }

            // shouldn't ever get here but stop the compiler from throwing a warning
            return static_cast<dtype>(histSize - 1);
        }

        //============================================================================
        // Method Description: 
        ///						Window expand around exceedance pixels
        ///		
        /// @param				NdArray<bool>
        /// @param				border width
        /// @return
        ///				NdArray<bool>
        ///
        static NdArray<bool> windowExceedances(const NdArray<bool>& inExceedances, uint8 inBorderWidth)
        {
            // not the most efficient way to do things, but the easist...
            NdArray<bool> xcds(inExceedances);
            Shape inShape = xcds.shape();
            for (uint8 border = 0; border < inBorderWidth; ++border)
            {
                for (int32 row = 0; row < static_cast<int32>(inShape.rows); ++row)
                {
                    for (int32 col = 0; col < static_cast<int32>(inShape.cols); ++col)
                    {
                        if (inExceedances(row, col))
                        {
                            xcds(std::max(row - 1, 0), std::max(col - 1, 0)) = true;
                            xcds(std::max(row - 1, 0), col) = true;
                            xcds(std::max(row - 1, 0), std::min<int32>(col + 1, inShape.cols - 1)) = true;

                            xcds(row, std::max<int32>(col - 1, 0)) = true;
                            xcds(row, std::min<int32>(col + 1, inShape.cols - 1)) = true;

                            xcds(std::min<int32>(row + 1, inShape.rows - 1), std::max(col - 1, 0)) = true;
                            xcds(std::min<int32>(row + 1, inShape.rows - 1), col) = true;
                            xcds(std::min<int32>(row + 1, inShape.rows - 1), std::min<int32>(col + 1, inShape.cols - 1)) = true;
                        }
                    }
                }
            }

            return std::move(xcds);
        }
    };
}
