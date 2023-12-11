/*
The MIT License

Copyright (c) 2023, Prominence AI, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in-
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _DSL_REMUXER_BINTR_H
#define _DSL_REMUXER_BINTR_H

#include "Dsl.h"
#include "DslApi.h"
#include "DslElementr.h"
#include "DslMultiBranchesBintr.h"
#include "DslBintr.h"

namespace DSL
{
    
    /**
     * @brief convenience macros for shared pointer abstraction
     */
    #define DSL_REMUXER_BRANCH_PTR std::shared_ptr<RemuxerBranchBintr>
    #define DSL_REMUXER_BRANCH_NEW(name, \
        parentRemuxerBin, pChildBranch, streamIds, numStreamIds) \
        std::shared_ptr<RemuxerBranchBintr>(new RemuxerBranchBintr(name, \
            parentRemuxerBin, pChildBranch, streamIds, numStreamIds))

    #define DSL_REMUXER_PTR std::shared_ptr<RemuxerBintr>
    #define DSL_REMUXER_NEW(name) \
        std::shared_ptr<RemuxerBintr>(new RemuxerBintr(name))

    /**
     * @class RemuxerBranchBintr
     * @brief Implements a Remuxer-Branch Proxy Bintr
     */
    class RemuxerBranchBintr : public Bintr
    {
    public: 
    
        /**
         * @brief ctor for the RemuxerBranchBintr class
         * @param[in] name name to give the new RemuxerBranchBintr
         * @param[in] parentRemuxerBin the GstObj for the Parent RemuxerBintr.
         * @param[in] pChildBranch child Branch for this RemuxerBranchBintr.
         * @param[in] streamIds array of stream-ids identifying the streams to
         * connect this RemuxerBranchBintr
         * @param[in] numStreamIds number of stream-ids in the streamIds array.
         */
        RemuxerBranchBintr(const char* name, GstObject* parentRemuxerBin, 
            DSL_BINTR_PTR pChildBranch, uint* streamIds, uint numStreamIds);
            
        /**
         * @brief dtor for the RemuxerBranchBintr class
         */
        ~RemuxerBranchBintr();

        /**
         * @brief Links all child components of this RemuxerBranchBintr
         * @return true if all components were succesfully linked, false otherwise.
         */
        bool LinkAll();
        
        /**
         * @brief Unlinks all child components of the RemuxerBintr
         */
        void UnlinkAll();
        
        /**
         * @brief Links the RemuxerBranchBintr to the Parent RemuxerBintr's
         * collection of tees, one for each streamId in m_streamIds.
         * @param splitters[in] vector of tees to link to.
         * @return true if all streams were successfully linked
         */
        bool LinkToSourceTees(const std::vector<DSL_ELEMENT_PTR>& tees);
        
        /**
         * @brief Unlinks the RemuxerBranchBintr from the Parent RemuxerBintr's
         * collection of tees.
         */
        void UnlinkFromSourceTees();

        /**
         * @brief Gets the current batch settings for the RemuxerBranchBintr's 
         * Streammuxer.
         * @return current batch-size, default == the number of source.
         */
        uint GetBatchSize();

        /**
         * @brief Sets the batch-size for the RemuxerBranchBintr's streammuxer. 
         * @param[in] batchSize new batchSize to set, default == the number of sources.
         * @return true if batch-properties are succesfully set, false otherwise.
         */
        bool SetBatchSize(uint batchSize);

        /**
         * @brief Gets the current config-file in use by the Pipeline's Streammuxer.
         * Default = NULL. Streammuxer will use all default vaules.
         * @return Current config file in use.
         */
        const char* GetStreammuxConfigFile();
        
        /**
         * @brief Sets the config-file for the Pipeline's Streammuxer to use.
         * Default = NULL. Streammuxer will use all default vaules.
         * @param[in] configFile absolute or relative pathspec to new Config file.
         * @return True if the config-file property could be set, false otherwise,
         */
        bool SetStreammuxConfigFile(const char* configFile);
        
    private:
    
        /**
         * @brief Child Streammuxer for the RemuxerBranchBintr
         */
        DSL_ELEMENT_PTR m_pStreammux;

        /**
         * @brief Child Branch to link to the Streammuxer
         */
        DSL_BINTR_PTR m_pChildBranch;
        
        /**
         * @brief Vector of stream-ids to connect this RemuxerBranchBintr to
         */
        std::vector<uint> m_streamIds;
        
        /**
         * @brief True if connecting to select stream-ids, false otherwise.
         */    
        bool m_linkSelectiveStreams;

        /**
         * @brief Absolute or relative path to the Streammuxer config file.
         */
        std::string m_streammuxConfigFile;

        /**
         * @brief Number of surfaces-per-frame stream-muxer setting
         */
        int m_numSurfacesPerFrame;

        /**
         * @brief Attach system timestamp as ntp timestamp, otherwise ntp 
         * timestamp calculated from RTCP sender reports.
         */
        boolean m_attachSysTs;
        
        /**
         * @brief if true, sychronizes input frames using PTS.
         */
        boolean m_syncInputs;
        
        /**
         * @brief The maximum upstream latency in nanoseconds. 
         * When sync-inputs=1, buffers coming in after max-latency shall be dropped.
         */
        uint m_maxLatency;

        /**
         * @brief Duration of input frames in milliseconds for use in NTP timestamp 
         * correction based on frame rate. If set to 0 (default), frame duration is 
         * inferred automatically from PTS values seen at RTP jitter buffer. When 
         * there is change in frame duration between the RTP jitter buffer and the 
         * nvstreammux, this property can be used to indicate the correct frame rate 
         * to the nvstreammux, for e.g. when there is an audiobuffersplit GstElement 
         * before nvstreammux in the pipeline. If set to -1, disables frame rate 
         * based NTP timestamp correction. 
         */
        int m_frameDuration;

        /**
         * @brief property to control EOS propagation downstream from nvstreammux
         * when all the sink pads are at EOS. (Experimental)
         */
        boolean m_dropPipelineEos;

        /**
         * @brief Container of Queues elements used to connect to Streammuxer.
         * The components are mapped by target stream-id
         */
        std::map<uint, DSL_ELEMENT_PTR> m_queues;
        
    };

    /**
     * @class RemuxerBintr
     * @brief Implements a Remuxer (demuxer-streammuxer) bin container
     */
    class RemuxerBintr : public TeeBintr
    {
    public: 
    
        /**
         * @brief ctor for the RemuxerBintr class
         * @param[in] name name to give the new RemuxerBintr
         */
        RemuxerBintr(const char* name);

        /**
         * @brief dtor for the RemuxerBintr class
         */
        ~RemuxerBintr();

        /**
         * @brief Adds this RemuxerBintr to a Parent Pipeline or Branch Bintr.
         * @param[in] pParentBintr parent Pipeline to add to
         * @return true on successful add, false otherwise
         */
        bool AddToParent(DSL_BASE_PTR pParentBintr);
        
        /**
         * @brief Removes this RemuxerBintr from a Parent Pipeline or Branch Bintr.
         * @param[in] pParentBintr parent Pipeline or Branch to remove from.
         * @return true on successful add, false otherwise
         */
        bool RemoveFromParent(DSL_BASE_PTR pParentBintr);

        /**
         * @brief Adds a child ComponentBintr to this RemuxerBintr. Each child 
         * (branch) is assigned a new Streammuxer. Each Streammuxer is connected
         * to streams produced by, and tee'd off of, the Bintr's Demuxer.
         * @return true if the ComponentBintr was added correctly, false otherwise
         */
        bool AddChild(DSL_BINTR_PTR pChildBranch);

        /**
         * @brief Adds a child ComponentBintr to this RemuxerBintr. Each child 
         * (branch) is assigned a new Streammuxer. Each Streammuxer is connected
         * to multiple streams produced by, and tee'd off of, the Bintr's Demuxer.
         * @param[in] pChildBranch shared pointer to Bintr (branch) to add.
         * @param[in] streamIds array of streamIds - identifing which streams to 
         * connect-to.
         * @return true if the ComponentBintr was added correctly, false otherwise
         */
        bool AddChildTo(DSL_BINTR_PTR pChildBranch, 
            uint* streamIds, uint numStreamIds);

        /**
         * @brief removes a child ComponentBintr from this RemuxerBintr
         * @param[in] pChildComponent a shared pointer to ComponentBintr to remove.
         * @return true if the ComponentBintr was removed correctly, false otherwise
         */
        bool RemoveChild(DSL_BINTR_PTR pChildComponent);
        
        /**
         * @brief overrides the base method and checks in m_pChildBranches only.
         */
        bool IsChild(DSL_BINTR_PTR pChildComponent);

        /**
         * @brief overrides the base Noder method to only return the number of 
         * child ComponentBintrs and not the total number of children... 
         * i.e. exclude the nuber of child Elementrs from the count
         * @return the number of Child ComponentBintrs (branches) held by this 
         * MultiBranchesBintr
         */
        uint GetNumChildren()
        {
            LOG_FUNC();
            
            return m_childBranches.size();
        }

        /**
         * @brief Links all child elements of this RemuxerBintr
         * @return true if all elements were succesfully linked, false otherwise.
         */
        bool LinkAll();
        
        /**
         * @brief Unlinks all child elements of the RemuxerBintr
         */
        void UnlinkAll();
        
        /**
         * @brief Overrides the base SetBatchSize() to set the batch size for 
         * this Bintr.
         * @param[in] batchSize the new batchSize to use.
         */
        bool SetBatchSize(uint batchSize);

        /**
         * @brief Gets the current batch settings for the RemuxerBintr.
         * @return Current batchSize, default == the number of sources, set by 
         * the parent once the Pipeline is playing, if not overridden.
         */
        uint GetBatchSize();

        /**
         * @brief Overrides the parent (branch) batchsize for the RemuxerBintr.
         * @param[in] batchSize new batchSize to set, default is set by parent.
         * @return true if batch-size is succesfully set, false otherwise.
         */
        bool OverrideBatchSize(uint batchSize);

        /**
         * @brief Gets the current config-file in use by the specified child branch.
         * Default = NULL. Streammuxer will use all default vaules.
         * @param[in] pChildComponent child branch to query.
         * @return Current config file in use.
         */
        const char* GetStreammuxConfigFile(DSL_BINTR_PTR pChildComponent);
        
        /**
         * @brief Sets the config-file for the Pipeline's Streammuxer to use.
         * Default = NULL. Streammuxer will use all default vaules.
         * @param[in] pChildComponent child branch to update.
         * @param[in] configFile absolute or relative pathspec to new Config file.
         * @return True if the config-file property could be set, false otherwise,
         */
        bool SetStreammuxConfigFile(DSL_BINTR_PTR pChildComponent,
            const char* configFile);
    
    private:
    
        /**
         * @brief Maximum number of stream-ids that can be connected.
         */
        uint m_maxStreamIds;
        
        /**
         * @brief true if batch-size explicity set by client, false by default.
         */
        bool m_batchSizeSetByClient;

        /**
         * @brief list of reguest pads -- batch-size in length -- for the 
         * for the DemuxerBintr. 
         * and then used on LinkAll or AddChild when in a linked-state
         */
        std::vector<GstPad*> m_requestedSrcPads;
        
        /**
         * @brief Streamdemuxer for the RemuxerBintr.
         */
        DSL_ELEMENT_PTR m_pDemuxer;
        
        /**
         * @brief Vector of Splitter Tees, one per maxStreamIds, for the RemuxerBintr.
         */
        std::vector<DSL_ELEMENT_PTR> m_tees;

        /**
         * @brief container of all child Sinks/Branches mapped by their unique names
         */
        std::map<std::string, DSL_REMUXER_BRANCH_PTR> m_childBranches;
    };
    
}

#endif // _DSL_REMUXER_BINTR_H