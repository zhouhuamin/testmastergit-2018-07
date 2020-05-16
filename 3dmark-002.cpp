
root@~:build32# git diff
diff --git a/qrenderdoc/Windows/Dialogs/LiveCapture.cpp b/qrenderdoc/Windows/Dialogs/LiveCapture.cpp
old mode 100644
new mode 100755
index 085c4c1b0..61ce6c9ca
--- a/qrenderdoc/Windows/Dialogs/LiveCapture.cpp
+++ b/qrenderdoc/Windows/Dialogs/LiveCapture.cpp
@@ -205,6 +205,11 @@ void LiveCapture::showEvent(QShowEvent *event)
   }
 
   on_captures_itemSelectionChanged();
+  //if (!m_ConnectThread2)
+  //{
+  //    m_ConnectThread2 = new LambdaThread([this]() { this->connectionThreadEntry2(); });
+  //    m_ConnectThread2->start();
+  //}
 }
 
 void LiveCapture::on_captures_mouseClicked(QMouseEvent *e)
@@ -1188,7 +1193,8 @@ void LiveCapture::connectionThreadEntry()
     ui->connectionStatus->setText(tr("Established"));
   });
 
-  while(m_Connection && m_Connection->Connected())
+  // while(m_Connection && m_Connection->Connected())
+  while (1)
   {
     if(m_TriggerCapture.tryAcquire())
     {
@@ -1249,6 +1255,9 @@ void LiveCapture::connectionThreadEntry()
       QString api = msg.apiUse.name;
       bool presenting = msg.apiUse.presenting;
       bool supported = msg.apiUse.supported;
+      uint32_t pid = msg.apiUse.pid;
+      qDebug("pid=%d", pid);
+
       GUIInvoke::call(this, [this, api, presenting, supported]() {
         m_APIs[api] = APIStatus(presenting, supported);
 
@@ -1261,6 +1270,43 @@ void LiveCapture::connectionThreadEntry()
 
         updateAPIStatus();
       });
+
+      if (m_pidSet.size() == 0) {
+          m_pidSet.insert(pid);
+      }
+      else {
+          auto it = m_pidSet.find(pid);
+          if (it != m_pidSet.end()) {
+
+          }
+          else {
+              m_pidSet.insert(pid);
+              m_connectionList.push_back(m_Connection);
+              m_Connection = RENDERDOC_CreateTargetControl(m_Hostname.toUtf8().data(), 38921,
+                  GetSystemUsername().toUtf8().data(), true);
+              if (!m_Connection || !m_Connection->Connected())
+              {
+                  qDebug("New Process connect failed!");
+                  return;
+              }
+              else {
+                  GUIInvoke::call(this, [this]() {
+                      if (!m_Connection)
+                          return;
+                      uint32_t pid = m_Connection->GetPID();
+                      QString target = QString::fromUtf8(m_Connection->GetTarget());
+                      if (pid)
+                          setTitle(QFormatStr("%1 [PID %2]").arg(target).arg(pid));
+                      else
+                          setTitle(target);
+
+                      ui->target->setText(windowTitle());
+                      ui->connectionIcon->setPixmap(Pixmaps::connect(ui->connectionIcon));
+                      ui->connectionStatus->setText(tr("Established"));
+                  });
+              }
+          }
+      }
     }
 
     if(msg.type == TargetControlMessageType::CaptureProgress)
@@ -1319,6 +1365,31 @@ void LiveCapture::connectionThreadEntry()
       uint32_t windows = msg.capturableWindowCount;
       GUIInvoke::call(this, [this, windows]() { ui->cycleActiveWindow->setEnabled(windows > 1); });
     }
+
+    if (m_connectionList.size() == 0) {
+
+    }
+    else {
+        if (!m_Connection || !m_Connection->Connected()) {
+            if (m_Connection)
+            {
+                m_Connection->Shutdown();
+                m_Connection = NULL;
+            }
+            m_Connection = m_connectionList.front();
+            m_connectionList.pop_back();
+            //auto &it = m_connectionList.back();
+            //if (it)
+            //{
+            //    it->Shutdown();
+            //    it = NULL;
+            //}
+            //m_connectionList.pop_back();
+        }
+    }
+    if (!m_Connection || !m_Connection->Connected()) {
+        break;
+    }
   }
 
   if(m_Connection)
@@ -1327,6 +1398,16 @@ void LiveCapture::connectionThreadEntry()
     m_Connection = NULL;
   }
 
+  //while (m_connectionList.size() > 0) {
+  //    auto &it = m_connectionList.back();
+  //    if (it)
+  //    {
+  //        it->Shutdown();
+  //        it = NULL;
+  //    }
+  //    m_connectionList.pop_back();
+  //}
+
   GUIInvoke::call(this, [this]() {
     ui->connectionStatus->setText(tr("Closed"));
     ui->connectionIcon->setPixmap(Pixmaps::disconnect(ui->connectionIcon));
@@ -1345,3 +1426,208 @@ void LiveCapture::connectionThreadEntry()
     connectionClosed();
   });
 }
+
+
+void LiveCapture::connectionThreadEntry2()
+{
+    while (1) {
+        // recv message ?
+        uint32_t pid = RENDERDOC_CreateTcpSocket(m_Hostname.toUtf8().data(), 37830,
+            GetSystemUsername().toUtf8().data(), true);
+        if (pid == 0) {
+            QThread::msleep(150);
+            continue;
+        }
+        else {
+            break;
+        }
+    }
+
+    m_Connection2 = RENDERDOC_CreateTargetControl(m_Hostname.toUtf8().data(), 38921,
+        GetSystemUsername().toUtf8().data(), true);
+
+    if (!m_Connection2 || !m_Connection2->Connected())
+    {
+        GUIInvoke::call(this, [this]() {
+            setTitle(tr("Connection failed"));
+            ui->connectionStatus->setText(tr("Failed"));
+            ui->connectionIcon->setPixmap(Pixmaps::del(ui->connectionIcon));
+
+            // connectionClosed();
+        });
+
+        return;
+    }
+
+    GUIInvoke::call(this, [this]() {
+        if (!m_Connection2)
+            return;
+        uint32_t pid = m_Connection2->GetPID();
+        QString target = QString::fromUtf8(m_Connection2->GetTarget());
+        if (pid)
+            setTitle(QFormatStr("%1 [PID %2]").arg(target).arg(pid));
+        else
+            setTitle(target);
+
+        ui->target->setText(windowTitle());
+        ui->connectionIcon->setPixmap(Pixmaps::connect(ui->connectionIcon));
+        ui->connectionStatus->setText(tr("Established"));
+    });
+
+    while (m_Connection2 && m_Connection2->Connected())
+    {
+        if (m_TriggerCapture.tryAcquire())
+        {
+            m_Connection2->TriggerCapture((uint)m_CaptureNumFrames);
+            m_CaptureNumFrames = 1;
+        }
+
+        if (m_QueueCapture.tryAcquire())
+        {
+            m_Connection2->QueueCapture((uint32_t)m_QueueCaptureFrameNum, (uint32_t)m_CaptureNumFrames);
+            m_QueueCaptureFrameNum = 0;
+            m_CaptureNumFrames = 1;
+        }
+
+        if (m_CopyCapture.tryAcquire())
+        {
+            m_Connection2->CopyCapture(m_CopyCaptureID, m_CopyCaptureLocalPath.toUtf8().data());
+            m_CopyCaptureLocalPath = QString();
+            m_CopyCaptureID = ~0U;
+        }
+
+        QVector<uint32_t> dels;
+        {
+            QMutexLocker l(&m_DeleteCapturesLock);
+            dels.swap(m_DeleteCaptures);
+        }
+
+        for (uint32_t del : dels)
+            m_Connection2->DeleteCapture(del);
+
+        if (!m_Disconnect.available())
+        {
+            m_Connection2->Shutdown();
+            m_Connection2 = NULL;
+            return;
+        }
+
+        TargetControlMessage msg = m_Connection2->ReceiveMessage([this](float progress) {
+            GUIInvoke::call(this, [this, progress]() {
+                if (progress >= 0.0f && progress < 1.0f)
+                {
+                    ui->progressLabel->setText(tr("Copy in Progress:"));
+                    ui->progressLabel->setVisible(true);
+                    ui->progressBar->setVisible(true);
+                    ui->progressBar->setMaximum(1000);
+                    ui->progressBar->setValue(1000 * progress);
+                }
+                else
+                {
+                    ui->progressLabel->setVisible(false);
+                    ui->progressBar->setVisible(false);
+                }
+            });
+        });
+
+        if (msg.type == TargetControlMessageType::RegisterAPI)
+        {
+            QString api = msg.apiUse.name;
+            bool presenting = msg.apiUse.presenting;
+            bool supported = msg.apiUse.supported;
+            GUIInvoke::call(this, [this, api, presenting, supported]() {
+                m_APIs[api] = APIStatus(presenting, supported);
+
+                if (presenting && supported)
+                {
+                    ui->triggerImmediateCapture->setEnabled(true);
+                    ui->triggerDelayedCapture->setEnabled(true);
+                    ui->queueCap->setEnabled(true);
+                }
+
+                updateAPIStatus();
+            });
+        }
+
+        if (msg.type == TargetControlMessageType::CaptureProgress)
+        {
+            float progress = msg.capProgress;
+            GUIInvoke::call(this, [this, progress]() {
+
+                if (progress >= 0.0f && progress < 1.0f)
+                {
+                    ui->progressLabel->setText(tr("Capture in Progress:"));
+                    ui->progressLabel->setVisible(true);
+                    ui->progressBar->setVisible(true);
+                    ui->progressBar->setMaximum(1000);
+                    ui->progressBar->setValue(1000 * progress);
+                }
+                else
+                {
+                    ui->progressLabel->setVisible(false);
+                    ui->progressBar->setVisible(false);
+                }
+
+            });
+        }
+
+        if (msg.type == TargetControlMessageType::NewCapture)
+        {
+            NewCaptureData cap = msg.newCapture;
+            GUIInvoke::call(this, [this, cap]() { captureAdded(cap); });
+        }
+
+        if (msg.type == TargetControlMessageType::CaptureCopied)
+        {
+            uint32_t capID = msg.newCapture.captureId;
+            QString path = msg.newCapture.path;
+
+            GUIInvoke::call(this, [this, capID, path]() { captureCopied(capID, path); });
+        }
+
+        if (msg.type == TargetControlMessageType::NewChild)
+        {
+            if (msg.newChild.processId != 0)
+            {
+                ChildProcess c;
+                c.PID = (int)msg.newChild.processId;
+                c.ident = msg.newChild.ident;
+
+                {
+                    QMutexLocker l(&m_ChildrenLock);
+                    m_Children.push_back(c);
+                }
+            }
+        }
+
+        if (msg.type == TargetControlMessageType::CapturableWindowCount)
+        {
+            uint32_t windows = msg.capturableWindowCount;
+            GUIInvoke::call(this, [this, windows]() { ui->cycleActiveWindow->setEnabled(windows > 1); });
+        }
+    }
+
+    if (m_Connection2)
+    {
+        m_Connection2->Shutdown();
+        m_Connection2 = NULL;
+    }
+
+    GUIInvoke::call(this, [this]() {
+        ui->connectionStatus->setText(tr("Closed"));
+        ui->connectionIcon->setPixmap(Pixmaps::disconnect(ui->connectionIcon));
+
+        ui->numFrames->setEnabled(false);
+        ui->captureDelay->setEnabled(false);
+        ui->captureFrame->setEnabled(false);
+        ui->triggerDelayedCapture->setEnabled(false);
+        ui->triggerImmediateCapture->setEnabled(false);
+        ui->queueCap->setEnabled(false);
+        ui->cycleActiveWindow->setEnabled(false);
+
+        ui->apiStatus->setText(tr("None"));
+        ui->apiIcon->setVisible(false);
+
+        // connectionClosed();
+    });
+}
diff --git a/qrenderdoc/Windows/Dialogs/LiveCapture.h b/qrenderdoc/Windows/Dialogs/LiveCapture.h
old mode 100644
new mode 100755
index 3a45e5e1f..0ce3c0910
--- a/qrenderdoc/Windows/Dialogs/LiveCapture.h
+++ b/qrenderdoc/Windows/Dialogs/LiveCapture.h
@@ -30,6 +30,8 @@
 #include <QSemaphore>
 #include <QTimer>
 #include "Code/Interface/QRDInterface.h"
+#include <set>
+#include <list>
 
 namespace Ui
 {
@@ -138,6 +140,7 @@ private:
   void updateAPIStatus();
 
   void connectionThreadEntry();
+  void connectionThreadEntry2();
   void captureCopied(uint32_t ID, const QString &localPath);
   void captureAdded(const NewCaptureData &newCapture);
   void connectionClosed();
@@ -160,6 +163,7 @@ private:
   MainWindow *m_Main;
 
   LambdaThread *m_ConnectThread = NULL;
+  LambdaThread *m_ConnectThread2 = NULL;
   QSemaphore m_TriggerCapture;
   QSemaphore m_QueueCapture;
   QSemaphore m_CopyCapture;
@@ -168,6 +172,7 @@ private:
   int m_QueueCaptureFrameNum = 0;
   int m_CaptureCounter = 0;
   ITargetControl *m_Connection = NULL;
+  ITargetControl *m_Connection2 = NULL;
 
   uint32_t m_CopyCaptureID = ~0U;
   QString m_CopyCaptureLocalPath;
@@ -192,4 +197,6 @@ private:
   QMutex m_ChildrenLock;
   QList<ChildProcess> m_Children;
   QMap<QString, APIStatus> m_APIs;
+  std::set<uint32_t> m_pidSet;
+  std::list<ITargetControl *> m_connectionList;
 };
diff --git a/renderdoc/android/android.cpp b/renderdoc/android/android.cpp
old mode 100644
new mode 100755
index f4689754b..1149cbc2b
--- a/renderdoc/android/android.cpp
+++ b/renderdoc/android/android.cpp
@@ -55,10 +55,31 @@ void adbForwardPorts(uint16_t portbase, const rdcstr &deviceID, uint16_t jdwpPor
                                    RenderDoc_FirstTargetControlPort),
                  ".", silent);
 
+  adbExecCommand(deviceID,
+      StringFormat::Fmt(forwardCommand, 38961,
+          38921),
+      ".", silent);
+
+  //adbExecCommand(deviceID,
+  //    StringFormat::Fmt(forwardCommand, RenderDoc_NewProcessTcpServerPort,
+  //        RenderDoc_NewProcessUnixServerPort),
+  //    ".", silent);
+
   if(jdwpPort && pid)
     adbExecCommand(deviceID, StringFormat::Fmt("forward tcp:%hu jdwp:%i", jdwpPort, pid));
 }
 
+void adbForwardPorts2(uint16_t portbase, const rdcstr &deviceID, uint16_t jdwpPort, int pid,
+    bool silent)
+{
+    const char *forwardCommand = "forward tcp:%i localabstract:renderdoc_%i";
+
+    adbExecCommand(deviceID,
+        StringFormat::Fmt(forwardCommand, portbase,
+            38921),
+        ".", silent);
+}
+
 uint16_t GetJdwpPort()
 {
   // we loop over a number of ports to try and avoid previous failed attempts from leaving sockets
@@ -956,6 +977,20 @@ struct AndroidController : public IDeviceProtocolHandler
     // we only support a single target control connection on android
     else if(srcPort == RenderDoc_FirstTargetControlPort)
       return portbase + RenderDoc_ForwardTargetControlOffset;
+    else if (srcPort == 38921)
+    {
+        // Android::adbForwardPorts2(38961, deviceID, 0, 0, true);
+        return 38961;
+    }
+    //else if (srcPort == 37830)
+    //{
+    //    return 37830;
+    //}
+    //else if (srcPort == RenderDoc_FirstTargetControlPort + 1)
+    //{
+    //    Android::adbForwardPorts2(38960, deviceID, 0, 0, true);
+    //    return portbase + RenderDoc_ForwardTargetControlOffset + 1;
+    //}
 
     return 0;
   }
diff --git a/renderdoc/android/jdwp.cpp b/renderdoc/android/jdwp.cpp
old mode 100644
new mode 100755
index 488b82f5e..8beefa587
--- a/renderdoc/android/jdwp.cpp
+++ b/renderdoc/android/jdwp.cpp
@@ -377,11 +377,12 @@ bool InjectLibraries(const rdcstr &deviceID, Network::Socket *sock)
     if(evData.eventKind == EventKind::MethodEntry)
       thread = evData.MethodEntry.thread;
   }
+ /*
 
-  if(thread == 0)
+  if (thread == 0)
   {
-    RDCERR("Didn't hit android.app.Application.onCreate()");
-    return false;
+      RDCERR("Didn't hit android.app.Application.onCreate()");
+      return false;
   }
 
   // find java.lang.Runtime
@@ -424,7 +425,7 @@ bool InjectLibraries(const rdcstr &deviceID, Network::Socket *sock)
     RDCERR("Failed to call load(%s/%s)!", libPath.c_str(), RENDERDOC_ANDROID_LIBRARY);
     return false;
   }
-
+ */
   return true;
 }
 };    // namespace JDWP
diff --git a/renderdoc/api/replay/control_types.h b/renderdoc/api/replay/control_types.h
old mode 100644
new mode 100755
index 3fa3301db..a413bf3bb
--- a/renderdoc/api/replay/control_types.h
+++ b/renderdoc/api/replay/control_types.h
@@ -506,6 +506,7 @@ struct APIUseData
 
   DOCUMENT("``True`` if the API can be captured.");
   bool supported = false;
+  uint32_t pid = 0;
 };
 
 DECLARE_REFLECTION_STRUCT(APIUseData);
diff --git a/renderdoc/api/replay/renderdoc_replay.h b/renderdoc/api/replay/renderdoc_replay.h
old mode 100644
new mode 100755
index 0dc00c04e..8213a1311
--- a/renderdoc/api/replay/renderdoc_replay.h
+++ b/renderdoc/api/replay/renderdoc_replay.h
@@ -1713,6 +1713,9 @@ This function will block until the control connection is ready, or an error occu
 extern "C" RENDERDOC_API ITargetControl *RENDERDOC_CC RENDERDOC_CreateTargetControl(
     const char *URL, uint32_t ident, const char *clientName, bool forceConnection);
 
+extern "C" RENDERDOC_API uint32_t RENDERDOC_CC RENDERDOC_CreateTcpSocket(
+    const char *URL, uint32_t ident, const char *clientName, bool forceConnection);
+
 DOCUMENT(R"(Repeatedly query to enumerate which targets are active on a given machine and their
 idents.
 
diff --git a/renderdoc/common/globalconfig.h b/renderdoc/common/globalconfig.h
old mode 100644
new mode 100755
index a80bebf72..a7472b1d4
--- a/renderdoc/common/globalconfig.h
+++ b/renderdoc/common/globalconfig.h
@@ -142,6 +142,8 @@ enum
   RenderDoc_FirstTargetControlPort = 38920,
   RenderDoc_LastTargetControlPort = RenderDoc_FirstTargetControlPort + 7,
   RenderDoc_RemoteServerPort = 39920,
+  RenderDoc_NewProcessUnixServerPort = 39990,
+  RenderDoc_NewProcessTcpServerPort = 37830,
 
   RenderDoc_ForwardPortBase = 38950,
   RenderDoc_ForwardTargetControlOffset = 0,
diff --git a/renderdoc/core/core.cpp b/renderdoc/core/core.cpp
old mode 100644
new mode 100755
index 17b6cf6d7..06419bcd9
--- a/renderdoc/core/core.cpp
+++ b/renderdoc/core/core.cpp
@@ -354,7 +354,7 @@ void RenderDoc::Initialise()
 
     uint32_t port = RenderDoc_FirstTargetControlPort;
 
-    Network::Socket *sock = Network::CreateServerSocket("0.0.0.0", port & 0xffff, 4);
+    Network::Socket *sock = Network::CreateServerSocket("0.0.0.0", port & 0xffff, 14);
 
     while(sock == NULL)
     {
@@ -365,7 +365,7 @@ void RenderDoc::Initialise()
         break;
       }
 
-      sock = Network::CreateServerSocket("0.0.0.0", port & 0xffff, 4);
+      sock = Network::CreateServerSocket("0.0.0.0", port & 0xffff, 14);
     }
 
     if(sock)
@@ -376,6 +376,10 @@ void RenderDoc::Initialise()
       m_RemoteThread = Threading::CreateThread([sock]() { TargetControlServerThread(sock); });
 
       RDCLOG("Listening for target control on %u", port);
+      if (port == RenderDoc_FirstTargetControlPort || port == RenderDoc_FirstTargetControlPort + 1) {
+          // send message to RenderDoc_NewProcessUnixServerPort;
+          Threading::CreateThread([sock]() { TargetControlServerThread2(sock); });
+      }
     }
     else
     {
diff --git a/renderdoc/core/core.h b/renderdoc/core/core.h
old mode 100644
new mode 100755
index c5919f390..72afb4950
--- a/renderdoc/core/core.h
+++ b/renderdoc/core/core.h
@@ -699,6 +699,7 @@ private:
 
   static void TargetControlServerThread(Network::Socket *sock);
   static void TargetControlClientThread(uint32_t version, Network::Socket *client);
+  static void TargetControlServerThread2(Network::Socket *sock);
 
   ICrashHandler *m_ExHandler;
 
diff --git a/renderdoc/core/remote_server.cpp b/renderdoc/core/remote_server.cpp
old mode 100644
new mode 100755
index ea7b39598..bac203e93
--- a/renderdoc/core/remote_server.cpp
+++ b/renderdoc/core/remote_server.cpp
@@ -47,7 +47,8 @@ RDOC_DEBUG_CONFIG(bool, RemoteServer_DebugLogging, false,
 static const uint32_t RemoteServerProtocolVersion =
     uint32_t(RENDERDOC_VERSION_MAJOR * 1000) | RENDERDOC_VERSION_MINOR;
 
-enum RemoteServerPacket
+uint32_t g_pid = 0;
+enum RemoteServerPacket : uint32_t
 {
   eRemoteServer_Noop = 1,
   eRemoteServer_Handshake,
@@ -80,6 +81,8 @@ enum RemoteServerPacket
   eRemoteServer_WriteSection,
   eRemoteServer_GetAvailableGPUs,
   eRemoteServer_RemoteServerCount,
+  eRemoteServer_NewProcess = 0x66,
+  eRemoteServer_GetPid = 0x67,
 };
 
 DECLARE_REFLECTION_ENUM(RemoteServerPacket);
@@ -123,6 +126,8 @@ rdcstr DoStringise(const RemoteServerPacket &el)
     STRINGISE_ENUM_NAMED(eRemoteServer_WriteSection, "WriteSection");
     STRINGISE_ENUM_NAMED(eRemoteServer_GetAvailableGPUs, "GetAvailableGPUs");
     STRINGISE_ENUM_NAMED(eRemoteServer_RemoteServerCount, "RemoteServerCount");
+    STRINGISE_ENUM_NAMED(eRemoteServer_NewProcess, "NewProcess");
+    STRINGISE_ENUM_NAMED(eRemoteServer_GetPid, "GetPid");
   }
   END_ENUM_STRINGISE();
 }
@@ -898,10 +903,324 @@ static void ActiveRemoteClientThread(ClientThread *threadData,
   SAFE_DELETE(client);
 }
 
+// add by zhouhm
+/*
+static void ActiveRemoteClientThread2(ClientThread *threadData)
+{
+    Threading::SetCurrentThreadName("ActiveRemoteClientThread2");
+
+    Network::Socket *&client = threadData->socket;
+
+    // client->SetTimeout(RemoteServer_TimeoutMS);
+
+    // uint32_t ip = client->GetRemoteIP();
+
+    // uint32_t version = 0;
+
+    
+      //{
+      //  ReadSerialiser ser(new StreamReader(client, Ownership::Nothing), Ownership::Stream);
+
+      //  // this thread just handles receiving the handshake and sending a busy signal without blocking
+      //  // the server thread
+      //  RemoteServerPacket type = ser.ReadChunk<RemoteServerPacket>();
+
+      //  if(ser.IsErrored() || type != eRemoteServer_Handshake)
+      //  {
+      //    RDCWARN("Didn't receive proper handshake");
+      //    SAFE_DELETE(client);
+      //    return;
+      //  }
+
+      //  SERIALISE_ELEMENT(version);
+
+      //  ser.EndChunk();
+      //}
+
+      //{
+      //  WriteSerialiser ser(new StreamWriter(client, Ownership::Nothing), Ownership::Stream);
+
+      //  ser.SetStreamingMode(true);
+
+      //  if(version != RemoteServerProtocolVersion)
+      //  {
+      //    RDCLOG("Connection using protocol %u, but we are running %u", version,
+      //           RemoteServerProtocolVersion);
+
+      //    {
+      //      SCOPED_SERIALISE_CHUNK(eRemoteServer_VersionMismatch);
+      //    }
+      //    SAFE_DELETE(client);
+      //    return;
+      //  }
+      //  else
+      //  {
+      //    // handshake and continue
+      //    SCOPED_SERIALISE_CHUNK(eRemoteServer_Handshake);
+      //  }
+      //}
+    
+
+    //rdcarray<rdcstr> tempFiles;
+    //IRemoteDriver *remoteDriver = NULL;
+    //IReplayDriver *replayDriver = NULL;
+    //ReplayProxy *proxy = NULL;
+    //RDCFile *rdc = NULL;
+    //Callstack::StackResolver *resolver = NULL;
+
+    WriteSerialiser writer(new StreamWriter(client, Ownership::Nothing), Ownership::Stream);
+    ReadSerialiser reader(new StreamReader(client, Ownership::Nothing), Ownership::Stream);
+
+    writer.SetStreamingMode(true);
+    reader.SetStreamingMode(true);
+
+    while (client)
+    {
+        if (client && !client->Connected())
+            break;
+
+        if (threadData->killThread)
+            break;
+
+        // this will block until a packet comes in.
+        RemoteServerPacket type = reader.ReadChunk<RemoteServerPacket>();
+
+        if (reader.IsErrored() || writer.IsErrored())
+            break;
+
+        if (client == NULL)
+            continue;
+        if (type == eRemoteServer_NewProcess)
+        {
+            {
+                READ_DATA_SCOPE();
+                SERIALISE_ELEMENT(g_pid);
+            }
+
+            reader.EndChunk();
+
+        }
+        else if (type == eRemoteServer_GetPid)
+        {
+            reader.EndChunk();
+            RDCLOG("message eRemoteServer_GetPid");
+
+            uint32_t pid = 0;
+            rdcstr path = "/sdcard/pidfile.txt";
+            FILE *file = FileIO::fopen(path.c_str(), "rb");
+            if (file == NULL) {
+                pid = 0;
+            }
+            else {
+                char buffer[10] = { 0 };
+                uint64_t filesize = FileIO::GetFileSize(path.c_str());
+                size_t read_ret = FileIO::fread(buffer, 1, (size_t)filesize, file);
+                FileIO::fclose(file);
+                if (read_ret != filesize) {
+                    pid = 0;
+                }
+                else {
+                    buffer[9] = '\0';
+                    pid = atoi(buffer);
+                }
+            }
+            g_pid = pid;
+
+            WRITE_DATA_SCOPE();
+            SCOPED_SERIALISE_CHUNK(eRemoteServer_GetPid);
+            SERIALISE_ELEMENT(g_pid);
+        }
+        else if (type == eRemoteServer_RemoteDriverList)
+        {
+            reader.EndChunk();
+
+            std::map<RDCDriver, rdcstr> drivers = RenderDoc::Inst().GetRemoteDrivers();
+            uint32_t count = (uint32_t)drivers.size();
+
+            WRITE_DATA_SCOPE();
+            SCOPED_SERIALISE_CHUNK(eRemoteServer_RemoteDriverList);
+            SERIALISE_ELEMENT(count);
+
+            for (auto it = drivers.begin(); it != drivers.end(); ++it)
+            {
+                RDCDriver driverType = it->first;
+                const rdcstr &driverName = it->second;
+
+                SERIALISE_ELEMENT(driverType);
+                SERIALISE_ELEMENT(driverName);
+            }
+        }
+        else if (type == eRemoteServer_HomeDir)
+        {
+            reader.EndChunk();
+
+            rdcstr home = FileIO::GetHomeFolderFilename();
+
+            {
+                WRITE_DATA_SCOPE();
+                SCOPED_SERIALISE_CHUNK(eRemoteServer_HomeDir);
+                SERIALISE_ELEMENT(home);
+            }
+        }
+        else if (type == eRemoteServer_ListDir)
+        {
+            rdcstr path;
+
+            {
+                READ_DATA_SCOPE();
+                SERIALISE_ELEMENT(path);
+            }
+
+            reader.EndChunk();
+
+            rdcarray<PathEntry> files;
+            FileIO::GetFilesInDirectory(path.c_str(), files);
+
+            {
+                WRITE_DATA_SCOPE();
+                SCOPED_SERIALISE_CHUNK(eRemoteServer_ListDir);
+                SERIALISE_ELEMENT(files);
+            }
+        }
+        else if (type == eRemoteServer_CopyCaptureFromRemote)
+        {
+            rdcstr path;
+
+            {
+                READ_DATA_SCOPE();
+                SERIALISE_ELEMENT(path);
+            }
+
+            reader.EndChunk();
+
+            {
+                WRITE_DATA_SCOPE();
+                SCOPED_SERIALISE_CHUNK(eRemoteServer_CopyCaptureFromRemote);
+
+                StreamReader fileStream(FileIO::fopen(path.c_str(), "rb"));
+                ser.SerialiseStream(path, fileStream);
+            }
+        }
+        else if (type == eRemoteServer_CopyCaptureToRemote)
+        {
+            rdcstr path;
+            rdcstr dummy, dummy2;
+            FileIO::GetDefaultFiles("remotecopy", path, dummy, dummy2);
+
+            RDCLOG("Copying file to local path '%s'.", path.c_str());
+
+            FileIO::CreateParentDirectory(path);
+
+            {
+                READ_DATA_SCOPE();
+
+                StreamWriter streamWriter(FileIO::fopen(path.c_str(), "wb"), Ownership::Stream);
+
+                ser.SerialiseStream(path.c_str(), streamWriter, NULL);
+            }
+
+            reader.EndChunk();
+
+            if (reader.IsErrored())
+            {
+                FileIO::Delete(path.c_str());
+
+                RDCERR("Network error receiving file");
+                break;
+            }
+
+            RDCLOG("File received.");
+
+            // tempFiles.push_back(path);
+
+            {
+                WRITE_DATA_SCOPE();
+                SCOPED_SERIALISE_CHUNK(eRemoteServer_CopyCaptureToRemote);
+                SERIALISE_ELEMENT(path);
+            }
+        }
+        else if (type == eRemoteServer_ShutdownServer)
+        {
+            reader.EndChunk();
+
+            RDCLOG("Requested to shut down.");
+
+            threadData->killServer = true;
+            threadData->killThread = true;
+
+            {
+                WRITE_DATA_SCOPE();
+                SCOPED_SERIALISE_CHUNK(eRemoteServer_ShutdownServer);
+            }
+        }
+    }
+
+    //SAFE_DELETE(proxy);
+
+    //if (remoteDriver)
+    //    remoteDriver->Shutdown();
+    //remoteDriver = NULL;
+    //replayDriver = NULL;
+    //SAFE_DELETE(rdc);
+    //SAFE_DELETE(resolver);
+
+    //for (size_t i = 0; i < tempFiles.size(); i++)
+    //{
+    //    FileIO::Delete(tempFiles[i].c_str());
+    //}
+
+    //RDCLOG("Closing active connection from %u.%u.%u.%u.", Network::GetIPOctet(ip, 0),
+    //    Network::GetIPOctet(ip, 1), Network::GetIPOctet(ip, 2), Network::GetIPOctet(ip, 3));
+
+    //RDCLOG("Ready for new active connection...");
+
+    SAFE_DELETE(client);
+}
+*/
+
+//static void BecomeRemoteServer2()
+//{
+//    Network::Socket *sock = Network::CreateServerSocket("", 39990, 1);
+//
+//    if (sock == NULL)
+//        return;
+//
+//    ClientThread *activeClientData = NULL;
+//    while (1)
+//    {
+//        Network::Socket *client = sock->AcceptClient(0);
+//
+//        uint32_t ip = client->GetRemoteIP();
+//
+//        RDCLOG("process BecomeRemoteServer2() Connection received from %u.%u.%u.%u.", Network::GetIPOctet(ip, 0),
+//            Network::GetIPOctet(ip, 1), Network::GetIPOctet(ip, 2), Network::GetIPOctet(ip, 3));
+//
+//        if (activeClientData == NULL)
+//        {
+//            activeClientData = new ClientThread();
+//            activeClientData->socket = client;
+//            // activeClientData->allowExecution = allowExecution;
+//
+//            activeClientData->thread = Threading::CreateThread([activeClientData]() {
+//                ActiveRemoteClientThread2(activeClientData);
+//            });
+//
+//            RDCLOG("Making active connection");
+//        }
+//    }
+//}
+// add end
+
 void RenderDoc::BecomeRemoteServer(const char *listenhost, uint16_t port,
                                    std::function<bool()> killReplay,
                                    RENDERDOC_PreviewWindowCallback previewWindow)
 {
+    //// add by zhouhm
+    //Threading::CreateThread([]() {
+    //    BecomeRemoteServer2();
+    //});
+    //// add end
+
   Network::Socket *sock = Network::CreateServerSocket(listenhost, port, 1);
 
   if(sock == NULL)
diff --git a/renderdoc/core/target_control.cpp b/renderdoc/core/target_control.cpp
old mode 100644
new mode 100755
index 7a44ab6ab..9c4f65c73
--- a/renderdoc/core/target_control.cpp
+++ b/renderdoc/core/target_control.cpp
@@ -72,7 +72,9 @@ enum PacketType : uint32_t
   ePacket_NewChild,
   ePacket_CaptureProgress,
   ePacket_CycleActiveWindow,
-  ePacket_CapturableWindowCount
+  ePacket_CapturableWindowCount,
+  ePacket_NewProcess = 0x66,
+  ePacket_GetPid = 0x67,
 };
 
 DECLARE_REFLECTION_ENUM(PacketType);
@@ -95,6 +97,8 @@ rdcstr DoStringise(const PacketType &el)
     STRINGISE_ENUM_NAMED(ePacket_CaptureProgress, "Capture Progress");
     STRINGISE_ENUM_NAMED(ePacket_CycleActiveWindow, "Cycle Active Window");
     STRINGISE_ENUM_NAMED(ePacket_CapturableWindowCount, "Capturable Window Count");
+    STRINGISE_ENUM_NAMED(ePacket_NewProcess, "New Process");
+    STRINGISE_ENUM_NAMED(ePacket_GetPid, "Get Pid");
   }
   END_ENUM_STRINGISE();
 }
@@ -116,7 +120,6 @@ void RenderDoc::TargetControlClientThread(uint32_t version, Network::Socket *cli
 
   rdcstr target = RenderDoc::Inst().GetCurrentTarget();
   uint32_t mypid = Process::GetCurrentPID();
-
   {
     WRITE_DATA_SCOPE();
     SCOPED_SERIALISE_CHUNK(ePacket_Handshake);
@@ -196,12 +199,32 @@ void RenderDoc::TargetControlClientThread(uint32_t version, Network::Socket *cli
       bool supported =
           RenderDoc::Inst().HasRemoteDriver(driver) || RenderDoc::Inst().HasReplayDriver(driver);
 
+      uint32_t pid = 0; //  Process::GetCurrentPID();
+      rdcstr path = "/sdcard/pidfile.txt";
+      FILE *file = FileIO::fopen(path.c_str(), "rb");
+      if (file == NULL) {
+          pid = 0;
+      }
+      else {
+          char buffer[10] = { 0 };
+          uint64_t filesize = FileIO::GetFileSize(path.c_str());
+          size_t read_ret = FileIO::fread(buffer, 1, (size_t)filesize, file);
+          FileIO::fclose(file);
+          if (read_ret != filesize) {
+              pid = 0;
+          }
+          else {
+              buffer[9] = '\0';
+              pid = atoi(buffer);
+          }
+      }
       WRITE_DATA_SCOPE();
       {
         SCOPED_SERIALISE_CHUNK(ePacket_APIUse);
         SERIALISE_ELEMENT(driver);
         SERIALISE_ELEMENT(presenting);
         SERIALISE_ELEMENT(supported);
+        SERIALISE_ELEMENT(pid);
       }
     }
     else if(caps.size() != captures.size())
@@ -512,6 +535,42 @@ void RenderDoc::TargetControlServerThread(Network::Socket *sock)
   Threading::ReleaseModuleExitThread();
 }
 
+// add by zhouhm
+void RenderDoc::TargetControlServerThread2(Network::Socket *sock)
+{
+    // RenderDoc_NewProcessUnixServerPort
+    // Network::Socket *client = sock->CreateUnixSocket(RenderDoc_NewProcessUnixServerPort, 4);
+    uint32_t mypid = Process::GetCurrentPID();
+
+    rdcstr path = "/sdcard/pidfile.txt";
+    FILE *file = FileIO::fopen(path.c_str(), "wb");
+    if (file == NULL) {
+    }
+    else {
+        char buffer[10] = { 0 };
+        snprintf(buffer, sizeof(buffer), "%d", mypid);
+        buffer[9] = '\0';
+        FileIO::fwrite(buffer, 1, strlen(buffer), file);
+        FileIO::fclose(file);
+    }
+
+    //RDCLOG("Enter TargetControlServerThread2() pid=%d, client=%p", mypid, client);
+    //{
+    //    WriteSerialiser ser(new StreamWriter(client, Ownership::Nothing), Ownership::Stream);
+
+    //    ser.SetStreamingMode(true);
+
+    //    {
+    //        SCOPED_SERIALISE_CHUNK(ePacket_NewProcess);
+    //        SERIALISE_ELEMENT(mypid);
+    //    }
+
+    //    // don't care about errors, we're going to close the connection either way
+    //    SAFE_DELETE(client);
+    //}
+}
+// add end
+
 struct TargetControl : public ITargetControl
 {
 public:
@@ -813,22 +872,26 @@ public:
       RDCDriver driver = RDCDriver::Unknown;
       bool presenting = false;
       bool supported = false;
+      uint32_t pid = 0;
 
       READ_DATA_SCOPE();
       SERIALISE_ELEMENT(driver);
       SERIALISE_ELEMENT(presenting);
       SERIALISE_ELEMENT(supported);
+      SERIALISE_ELEMENT(pid);
+      // m_PID = pid;
 
       msg.apiUse.name = ToStr(driver);
       msg.apiUse.presenting = presenting;
       msg.apiUse.supported = supported;
+      msg.apiUse.pid = pid;
 
       if(presenting)
         m_API = ToStr(driver);
 
-      RDCLOG("Used API: %s (%s & %s)", msg.apiUse.name.c_str(),
+      RDCLOG("Used API: %s (%s & %s %d)", msg.apiUse.name.c_str(),
              presenting ? "Presenting" : "Not presenting",
-             supported ? "supported" : "not supported");
+             supported ? "supported" : "not supported", pid);
 
       reader.EndChunk();
       return msg;
@@ -924,3 +987,60 @@ extern "C" RENDERDOC_API ITargetControl *RENDERDOC_CC RENDERDOC_CreateTargetCont
   delete remote;
   return NULL;
 }
+
+// add by zhouhm
+extern "C" RENDERDOC_API uint32_t RENDERDOC_CC RENDERDOC_CreateTcpSocket(
+    const char *URL, uint32_t ident, const char *clientName, bool forceConnection)
+{
+    rdcstr host = "localhost";
+    if (URL != NULL && URL[0] != '\0')
+        host = URL;
+
+    rdcstr deviceID = host;
+    uint16_t port = ident & 0xffff;
+
+    IDeviceProtocolHandler *protocol = RenderDoc::Inst().GetDeviceProtocol(deviceID);
+
+    if (protocol)
+    {
+        deviceID = protocol->GetDeviceID(deviceID);
+        host = protocol->RemapHostname(deviceID);
+        if (host.empty())
+            return 0;
+
+        port = protocol->RemapPort(deviceID, port);
+    }
+
+    Network::Socket *sock = Network::CreateClientSocket(host.c_str(), port, 750);
+
+    if (sock == NULL)
+        return 0;
+    if (!sock->Connected())
+        return 0;
+    uint32_t pid = 0;
+    {
+        WriteSerialiser writer(new StreamWriter(sock, Ownership::Nothing), Ownership::Stream);
+        ReadSerialiser reader(new StreamReader(sock, Ownership::Nothing), Ownership::Stream);
+        writer.SetStreamingMode(true);
+        reader.SetStreamingMode(true);
+
+        {
+            WRITE_DATA_SCOPE();
+            SCOPED_SERIALISE_CHUNK(ePacket_GetPid);
+        }
+
+        PacketType type = reader.ReadChunk<PacketType>();
+
+        reader.EndChunk();
+
+        if (type == ePacket_GetPid)
+        {
+            READ_DATA_SCOPE();
+            SERIALISE_ELEMENT(pid);
+        }
+    }
+    return pid;
+
+}
+// add end
+
diff --git a/renderdoc/driver/gl/egl_hooks.cpp b/renderdoc/driver/gl/egl_hooks.cpp
old mode 100644
new mode 100755
index a6af89cfb..01a476d80
--- a/renderdoc/driver/gl/egl_hooks.cpp
+++ b/renderdoc/driver/gl/egl_hooks.cpp
@@ -229,6 +229,7 @@ HOOK_EXPORT EGLContext EGLAPIENTRY eglCreateContext_renderdoc_hooked(EGLDisplay
                                                                      EGLContext shareContext,
                                                                      EGLint const *attribList)
 {
+    EGLConfig tmpConfig = config;
   if(RenderDoc::Inst().IsReplayApp())
   {
     if(!EGL.CreateContext)
@@ -319,12 +320,27 @@ HOOK_EXPORT EGLContext EGLAPIENTRY eglCreateContext_renderdoc_hooked(EGLDisplay
   init.height = 0;
 
   EGLint value;
-  EGL.GetConfigAttrib(display, config, EGL_BUFFER_SIZE, &value);
+  EGL.GetConfigAttrib(display, tmpConfig, EGL_BUFFER_SIZE, &value);
   init.colorBits = value;
-  EGL.GetConfigAttrib(display, config, EGL_DEPTH_SIZE, &value);
+  if (init.colorBits > 32)
+  {
+      RDCLOG("process eglCreateContext_renderdoc_hooked() colorBits=%d, errno=%d", init.colorBits, EGL.GetError());
+  }
+
+  EGL.GetConfigAttrib(display, tmpConfig, EGL_DEPTH_SIZE, &value);
   init.depthBits = value;
-  EGL.GetConfigAttrib(display, config, EGL_STENCIL_SIZE, &value);
+  if (init.depthBits > 32)
+  {
+      RDCLOG("process eglCreateContext_renderdoc_hooked() depthBits=%d, errno=%d", init.depthBits, EGL.GetError());
+  }
+
+  EGL.GetConfigAttrib(display, tmpConfig, EGL_STENCIL_SIZE, &value);
   init.stencilBits = value;
+  if (init.stencilBits > 32)
+  {
+      RDCLOG("process eglCreateContext_renderdoc_hooked() depthBits=%d, errno=%d", init.stencilBits, EGL.GetError());
+  }
+
   // We will set isSRGB when we see the surface.
   init.isSRGB = 0;
 
diff --git a/renderdoc/driver/gl/gl_hooks.cpp b/renderdoc/driver/gl/gl_hooks.cpp
old mode 100644
new mode 100755
diff --git a/renderdoc/driver/gl/wrappers/gl_get_funcs.cpp b/renderdoc/driver/gl/wrappers/gl_get_funcs.cpp
old mode 100644
new mode 100755
index 06f1d3af4..8280092df
--- a/renderdoc/driver/gl/wrappers/gl_get_funcs.cpp
+++ b/renderdoc/driver/gl/wrappers/gl_get_funcs.cpp
@@ -789,14 +789,14 @@ void WrappedOpenGL::glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsi
 
 const GLubyte *WrappedOpenGL::glGetString(GLenum name)
 {
-  if(name == eGL_EXTENSIONS)
-  {
-    return (const GLubyte *)GetCtxData().glExtsString.c_str();
-  }
-  else if(name == eGL_DEBUG_TOOL_NAME_EXT)
-  {
-    return (const GLubyte *)"RenderDoc";
-  }
+  //if(name == eGL_EXTENSIONS)
+  //{
+  //  return (const GLubyte *)GetCtxData().glExtsString.c_str();
+  //}
+  //else if(name == eGL_DEBUG_TOOL_NAME_EXT)
+  //{
+  //  return (const GLubyte *)"RenderDoc";
+  //}
   return GL.glGetString(name);
 }
 
diff --git a/renderdoc/os/os_specific.h b/renderdoc/os/os_specific.h
old mode 100644
new mode 100755
index 1a4a5eb93..bd086b885
--- a/renderdoc/os/os_specific.h
+++ b/renderdoc/os/os_specific.h
@@ -176,6 +176,7 @@ public:
   uint32_t GetTimeout() const { return timeoutMS; }
   void SetTimeout(uint32_t milliseconds) { timeoutMS = milliseconds; }
   Socket *AcceptClient(uint32_t timeoutMilliseconds);
+  Socket *CreateUnixSocket(uint16_t port, int queuesize);
 
   uint32_t GetRemoteIP() const;
 
diff --git a/renderdoc/os/posix/posix_network.cpp b/renderdoc/os/posix/posix_network.cpp
old mode 100644
new mode 100755
index 5e0ac1b89..cfb51ae75
--- a/renderdoc/os/posix/posix_network.cpp
+++ b/renderdoc/os/posix/posix_network.cpp
@@ -140,6 +140,39 @@ Socket *Socket::AcceptClient(uint32_t timeoutMilliseconds)
   return NULL;
 }
 
+// add by zhouhm
+Socket *Socket::CreateUnixSocket(uint16_t port, int queuesize)
+{
+    int s = ::socket(AF_UNIX, SOCK_STREAM, 0);
+    if (s == -1)
+    {
+        RDCWARN("Unable to create unix socket");
+        return NULL;
+    }
+
+    rdcstr socketName = StringFormat::Fmt("renderdoc_%d", port);
+
+    sockaddr_un addr;
+    RDCEraseEl(addr);
+
+    addr.sun_family = AF_UNIX;
+    // first char is '\0'
+    addr.sun_path[0] = '\0';
+    strncpy(addr.sun_path + 1, socketName.c_str(), socketName.size() + 1);
+
+    int result = connect(s, (sockaddr *)&addr, offsetof(sockaddr_un, sun_path) + 1 + socketName.size());
+    if (result == -1)
+    {
+        RDCWARN("can not connect to the server: %s", socketName.c_str());
+        close(s);
+        return NULL;
+    }
+    RDCLOG("Created  socket: %d", s);
+
+    return new Socket((ptrdiff_t)s);
+}
+// add end
+
 bool Socket::SendDataBlocking(const void *buf, uint32_t length)
 {
   if(length == 0)
diff --git a/renderdoc/os/win32/win32_network.cpp b/renderdoc/os/win32/win32_network.cpp
old mode 100644
new mode 100755
index e67a7a921..d82225c3d
--- a/renderdoc/os/win32/win32_network.cpp
+++ b/renderdoc/os/win32/win32_network.cpp
@@ -180,6 +180,11 @@ Socket *Socket::AcceptClient(uint32_t timeoutMilliseconds)
   return NULL;
 }
 
+Socket *Socket::CreateUnixSocket(uint16_t port, int queuesize)
+{
+    return nullptr;
+}
+
 bool Socket::SendDataBlocking(const void *buf, uint32_t length)
 {
   if(length == 0)
root@~:build32# 