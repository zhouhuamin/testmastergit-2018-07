
int WrappedVulkan::SnapshotDraw()
{

    //if(!IsActiveCapturing(m_State))
    //  return true;

    //VkSwapchainKHR swap = VK_NULL_HANDLE;

    
      //if(wnd)
      //{
      //  {
      //    SCOPED_LOCK(m_SwapLookupLock);
      //    auto it = m_SwapLookup.find(wnd);
      //    if(it != m_SwapLookup.end())
      //      swap = it->second;
      //  }

      //  if(swap == VK_NULL_HANDLE)
      //  {
      //    RDCERR("Output window %p provided for frame capture corresponds with no known swap chain", wnd);
      //    return false;
      //  }
      //}

    //VkResourceRecord *swaprecord = NULL;

    //if (swap != VK_NULL_HANDLE)
    //{
    //    GetResourceManager()->MarkResourceFrameReferenced(GetResID(swap), eFrameRef_Read);

    //    swaprecord = GetRecord(swap);
    //    RDCASSERT(swaprecord->swapInfo);

    //    const SwapchainInfo &swapInfo = *swaprecord->swapInfo;

    //    backbuffer = swapInfo.images[swapInfo.lastPresent].im;

    //    // mark all images referenced as well
    //    for (size_t i = 0; i < swapInfo.images.size(); i++)
    //        GetResourceManager()->MarkResourceFrameReferenced(GetResID(swapInfo.images[i].im),
    //            eFrameRef_Read);
    //}
    //else
    //{
    //    // if a swapchain wasn't specified or found, use the last one presented
    //    //swaprecord = GetResourceManager()->GetResourceRecord(m_LastSwap);

    //    ////if (swaprecord)
    //    //{
    //    //    //GetResourceManager()->MarkResourceFrameReferenced(swaprecord->GetResourceID(), eFrameRef_Read);
    //    //    //RDCASSERT(swaprecord->swapInfo);

    //    //    const SwapchainInfo &swapInfo = m_CreationInfo.m_SwapChain[m_currentId];  // *swaprecord->swapInfo;

    //    //    backbuffer = swapInfo.images[swapInfo.lastPresent].im;

    //    //    // mark all images referenced as well
    //    //    for (size_t i = 0; i < swapInfo.images.size(); i++)
    //    //        GetResourceManager()->MarkResourceFrameReferenced(GetResID(swapInfo.images[i].im),
    //    //            eFrameRef_Read);
    //    //}
    //}

    //// transition back to IDLE atomically
    //{
    //    //SCOPED_LOCK(m_CapTransitionLock);
    //    //EndCaptureFrame(backbuffer);

    //    //m_State = CaptureState::BackgroundCapturing;

    //    // m_SuccessfulCapture = false;

    //    //ObjDisp(GetDev())->DeviceWaitIdle(Unwrap(GetDev()));

    //    //{
    //    //    SCOPED_LOCK(m_CoherentMapsLock);
    //    //    for (auto it = m_CoherentMaps.begin(); it != m_CoherentMaps.end(); ++it)
    //    //    {
    //    //        FreeAlignedBuffer((*it)->memMapState->refData);
    //    //        (*it)->memMapState->refData = NULL;
    //    //        (*it)->memMapState->needRefData = false;
    //    //    }
    //    //}
    //}

    VkImage backbuffer = VK_NULL_HANDLE;
    // gather backbuffer screenshot
    const uint32_t maxSize = 2048;
    RenderDoc::FramePixels fp;

    //if(swap != VK_NULL_HANDLE)
    //if (swaprecord != NULL)
    {
        //VkDevice device = GetDev();
        //if (device == NULL)
        //    return 0;
        //VkCommandBuffer cmd = GetNextCmd();

        //const VkLayerDispatchTable *vt = ObjDisp(device);

        //vt->DeviceWaitIdle(Unwrap(device));

        RDCLOG("Process WrappedVulkan::SnapshotDraw() m_currentId=%ld", m_currentId.id);
        //const SwapchainInfo &swapInfo = *swaprecord->swapInfo;
        const SwapchainInfo &swapInfo = m_CreationInfo.m_SwapChain[m_currentId];    // *swaprecord->swapInfo;
        //backbuffer = swapInfo.images[swapInfo.lastPresent].im;
        for (int i = 0; i < swapInfo.images.size(); ++i)
        {
            VkDevice device = GetDev();
            if (device == NULL)
                return 0;
            VkCommandBuffer cmd = GetNextCmd();

            const VkLayerDispatchTable *vt = ObjDisp(device);

            vt->DeviceWaitIdle(Unwrap(device));
            backbuffer = swapInfo.images[i].im;
            RDCLOG("Process WrappedVulkan::SnapshotDraw() wndHandle=%p, lastPresent=%d", swapInfo.wndHandle, swapInfo.lastPresent);

            // since this happens during capture, we don't want to start serialising extra buffer creates,
            // so we manually create & then just wrap.
            VkBuffer readbackBuf = VK_NULL_HANDLE;

            VkResult vkr = VK_SUCCESS;

            // create readback buffer
            VkBufferCreateInfo bufInfo = {
                VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                NULL,
                0,
                GetByteSize(swapInfo.extent.width, swapInfo.extent.height, 1, swapInfo.format, 0),
                VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            };
            vt->CreateBuffer(Unwrap(device), &bufInfo, NULL, &readbackBuf);
            RDCASSERTEQUAL(vkr, VK_SUCCESS);

            GetResourceManager()->WrapResource(Unwrap(device), readbackBuf);

            MemoryAllocation readbackMem =
                AllocateMemoryForResource(readbackBuf, MemoryScope::InitialContents, MemoryType::Readback);

            vkr = vt->BindBufferMemory(Unwrap(device), Unwrap(readbackBuf), Unwrap(readbackMem.mem),
                readbackMem.offs);
            RDCASSERTEQUAL(vkr, VK_SUCCESS);

            VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, NULL,
                                                  VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT };

            // do image copy
            vkr = vt->BeginCommandBuffer(Unwrap(cmd), &beginInfo);
            RDCASSERTEQUAL(vkr, VK_SUCCESS);

            uint32_t rowPitch = GetByteSize(swapInfo.extent.width, 1, 1, swapInfo.format, 0);

            VkBufferImageCopy cpy = {
                0,
                0,
                0,
                {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
                {
                    0, 0, 0,
                },
                {swapInfo.extent.width, swapInfo.extent.height, 1},
            };

            uint32_t swapQueueIndex = m_ImageLayouts[GetResID(backbuffer)].queueFamilyIndex;

            VkImageMemoryBarrier bbBarrier = {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                NULL,
                0,
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                swapQueueIndex,
                m_QueueFamilyIdx,
                Unwrap(backbuffer),
                {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
            };

            if (swapInfo.shared)
                bbBarrier.oldLayout = VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR;

            DoPipelineBarrier(cmd, 1, &bbBarrier);

            if (swapQueueIndex != m_QueueFamilyIdx)
            {
                VkCommandBuffer extQCmd = GetExtQueueCmd(swapQueueIndex);

                vkr = vt->BeginCommandBuffer(Unwrap(extQCmd), &beginInfo);
                RDCASSERTEQUAL(vkr, VK_SUCCESS);

                DoPipelineBarrier(extQCmd, 1, &bbBarrier);

                ObjDisp(extQCmd)->EndCommandBuffer(Unwrap(extQCmd));

                SubmitAndFlushExtQueue(swapQueueIndex);
            }

            vt->CmdCopyImageToBuffer(Unwrap(cmd), Unwrap(backbuffer), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                Unwrap(readbackBuf), 1, &cpy);

            // barrier to switch backbuffer back to present layout
            std::swap(bbBarrier.oldLayout, bbBarrier.newLayout);
            std::swap(bbBarrier.srcAccessMask, bbBarrier.dstAccessMask);
            std::swap(bbBarrier.srcQueueFamilyIndex, bbBarrier.dstQueueFamilyIndex);

            VkBufferMemoryBarrier bufBarrier = {
                VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                NULL,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_ACCESS_HOST_READ_BIT,
                VK_QUEUE_FAMILY_IGNORED,
                VK_QUEUE_FAMILY_IGNORED,
                Unwrap(readbackBuf),
                0,
                bufInfo.size,
            };

            DoPipelineBarrier(cmd, 1, &bbBarrier);
            DoPipelineBarrier(cmd, 1, &bufBarrier);

            vkr = vt->EndCommandBuffer(Unwrap(cmd));
            RDCASSERTEQUAL(vkr, VK_SUCCESS);

            SubmitCmds();
            FlushQ();    // need to wait so we can readback

            if (swapQueueIndex != m_QueueFamilyIdx)
            {
                VkCommandBuffer extQCmd = GetExtQueueCmd(swapQueueIndex);

                vkr = vt->BeginCommandBuffer(Unwrap(extQCmd), &beginInfo);
                RDCASSERTEQUAL(vkr, VK_SUCCESS);

                DoPipelineBarrier(extQCmd, 1, &bbBarrier);

                ObjDisp(extQCmd)->EndCommandBuffer(Unwrap(extQCmd));

                SubmitAndFlushExtQueue(swapQueueIndex);
            }

            // map memory and readback
            byte *pData = NULL;
            vkr = vt->MapMemory(Unwrap(device), Unwrap(readbackMem.mem), readbackMem.offs, readbackMem.size,
                0, (void **)&pData);
            RDCASSERTEQUAL(vkr, VK_SUCCESS);
            RDCASSERT(pData != NULL);

            fp.len = (uint32_t)readbackMem.size;
            fp.data = new uint8_t[fp.len];
            memcpy(fp.data, pData, fp.len);

            VkMappedMemoryRange range = {
                VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                NULL,
                Unwrap(readbackMem.mem),
                readbackMem.offs,
                readbackMem.size,
            };

            vkr = vt->InvalidateMappedMemoryRanges(Unwrap(device), 1, &range);
            RDCASSERTEQUAL(vkr, VK_SUCCESS);

            vt->UnmapMemory(Unwrap(device), Unwrap(readbackMem.mem));

            // delete all
            vt->DestroyBuffer(Unwrap(device), Unwrap(readbackBuf), NULL);
            GetResourceManager()->ReleaseWrappedResource(readbackBuf);

            ResourceFormat fmt = MakeResourceFormat(swapInfo.format);
            fp.width = swapInfo.extent.width;
            fp.height = swapInfo.extent.height;
            fp.pitch = rowPitch;
            fp.stride = fmt.compByteWidth * fmt.compCount;
            fp.bpc = fmt.compByteWidth;
            fp.bgra = fmt.BGRAOrder();
            fp.max_width = maxSize;
            fp.pitch_requirement = 8;
            switch (fmt.type)
            {
            case ResourceFormatType::R10G10B10A2:
                fp.stride = 4;
                fp.buf1010102 = true;
                break;
            case ResourceFormatType::R5G6B5:
                fp.stride = 2;
                fp.buf565 = true;
                break;
            case ResourceFormatType::R5G5B5A1:
                fp.stride = 2;
                fp.buf5551 = true;
                break;
            default: break;
            }

            //if (type == 1)
            {
                RDCThumb outRaw, outPng;
                if (fp.data)
                {
                    // point sample info into raw buffer
                    RenderDoc::Inst().ResamplePixels(fp, outRaw);
                    RenderDoc::Inst().EncodePixelsPNG(outRaw, outPng);

                    BinaryThumbnail2 thumbHeader = { 0 };

                    thumbHeader.width = outPng.width;
                    thumbHeader.height = outPng.height;
                    const byte *jpgPixels = outPng.pixels;
                    thumbHeader.length = outPng.len;

                    byte *jpgBuffer = NULL;
                    if (outPng.format != FileType::JPG && outPng.width > 0 && outPng.height > 0)
                    {
                        // the primary thumbnail must be in JPG format, must perform conversion
                        const byte *rawPixels = NULL;
                        byte *rawBuffer = NULL;
                        int w = (int)outPng.width;
                        int h = (int)outPng.height;
                        int comp = 3;

                        if (outPng.format == FileType::Raw)
                        {
                            rawPixels = outPng.pixels;
                        }
                        else
                        {
                            rawBuffer = stbi_load_from_memory(outPng.pixels, (int)outPng.len, &w, &h, &comp, 3);
                            rawPixels = rawBuffer;
                        }

                        if (rawPixels)
                        {
                            int len = w * h * comp;
                            jpgBuffer = new byte[len];
                            jpge::params p;
                            p.m_quality = 90;
                            jpge::compress_image_to_jpeg_file_in_memory(jpgBuffer, len, w, h, comp, rawPixels, p);
                            thumbHeader.length = (uint32_t)len;
                            jpgPixels = jpgBuffer;
                        }
                        else
                        {
                            thumbHeader.width = 0;
                            thumbHeader.height = 0;
                            thumbHeader.length = 0;
                            jpgPixels = NULL;
                        }
                        if (rawBuffer)
                            stbi_image_free(rawBuffer);
                    }

                    {
                        if (thumbHeader.length > 0)
                        {
                            static int nSeq = 0;
                            char szFileName[256] = { 0 };
                            snprintf(szFileName, sizeof(szFileName), "E:\\4-logs\\8-happyElements\\pics\\%d.png", nSeq++);
                            //snprintf(szFileName, sizeof(szFileName), "/sdcard/apitrace/%d.png", nSeq++);
                            FILE *pFile = FileIO::fopen(szFileName, "wb");
                            if (pFile != NULL)
                            {
                                FileIO::fwrite(jpgPixels, 1, thumbHeader.length, pFile);
                                FileIO::fclose(pFile);
                            }
                        }

                        delete[] jpgBuffer;
                    }
                    SAFE_DELETE_ARRAY(outRaw.pixels);
                    SAFE_DELETE_ARRAY(outPng.pixels);
                }
            }
        }
    }
    return 1;
}

