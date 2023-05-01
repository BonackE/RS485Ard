#include "MyApp.h"

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 850

JSValueRef LoadPorts(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount,
    const JSValueRef arguments[], JSValueRef* exception) {
    
    //Get current view of window
	
	//Get current window

    
    
    return JSModbusWrapper::GetInstance().LoadPortsFunc(ctx);
}
JSValueRef Connect(JSContextRef ctx, JSObjectRef function,
    JSObjectRef thisObject, size_t argumentCount,
    const JSValueRef arguments[], JSValueRef* exception) {
	return JSModbusWrapper::GetInstance().ConnectFunc(ctx);
}
JSValueRef Requests(JSContextRef ctx, JSObjectRef function,
	JSObjectRef thisObject, size_t argumentCount,
	const JSValueRef arguments[], JSValueRef* exception) {
	return JSModbusWrapper::GetInstance().RequestFunc(ctx);
}

MyApp::MyApp() {
  ///
  /// Create our main App instance.
  ///
  app_ = App::Create();
  JSModbusWrapper& jsmodbus = JSModbusWrapper::GetInstance();
  ///
  /// Create a resizable window by passing by OR'ing our window flags with
  /// kWindowFlags_Resizable.
  ///
  window_ = Window::Create(app_->main_monitor(), WINDOW_WIDTH, WINDOW_HEIGHT,
    false, kWindowFlags_Titled);

  ///
  /// Create our HTML overlay-- we don't care about its initial size and
  /// position because it'll be calculated when we call OnResize() below.
  ///
  overlay_ = Overlay::Create(window_, 1, 1, 0, 0);

  ///
  /// Force a call to OnResize to perform size/layout of our overlay.
  ///
  OnResize(window_.get(), window_->width(), window_->height());

  ///
  /// Load a page into our overlay's View
  ///
  overlay_->view()->LoadURL("file:///app.html");

  

  ///
  /// Register our MyApp instance as an AppListener so we can handle the
  /// App's OnUpdate event below.
  ///
  app_->set_listener(this);

  ///
  /// Register our MyApp instance as a WindowListener so we can handle the
  /// Window's OnResize event below.
  ///
  window_->set_listener(this);

  ///
  /// Register our MyApp instance as a LoadListener so we can handle the
  /// View's OnFinishLoading and OnDOMReady events below.
  ///
  overlay_->view()->set_load_listener(this);

  ///
  /// Register our MyApp instance as a ViewListener so we can handle the
  /// View's OnChangeCursor and OnChangeTitle events below.
  ///
  overlay_->view()->set_view_listener(this);
}

MyApp::~MyApp() {
}

void MyApp::Run() {
  app_->Run();
}

void MyApp::OnUpdate() {
  ///
  /// This is called repeatedly from the application's update loop.
  ///
  /// You should update any app logic here.
  ///
}

void MyApp::OnClose(ultralight::Window* window) {
  app_->Quit();
}

void MyApp::OnResize(ultralight::Window* window, uint32_t width, uint32_t height) {
  ///
  /// This is called whenever the window changes size (values in pixels).
  ///
  /// We resize our overlay here to take up the entire window.
  ///
  overlay_->Resize(width, height);
}

void MyApp::OnFinishLoading(ultralight::View* caller,
                            uint64_t frame_id,
                            bool is_main_frame,
                            const String& url) {
  ///
  /// This is called when a frame finishes loading on the page.
  ///
}

void MyApp::OnDOMReady(ultralight::View* caller,
    uint64_t frame_id,
    bool is_main_frame,
    const String& url) {
    RefPtr<JSContext> context = caller->LockJSContext();
	//Send caller to JSModbusWrapper
    

    // Get the underlying JSContextRef for use with the
    // JavaScriptCore C API.
    JSContextRef ctx = *context.get();
	JSModbusWrapper::GetInstance().SetView(caller);
    // Create a JavaScript String containing the name of our callback.
    JSStringRef LoadPJ = JSStringCreateWithUTF8CString("LoadPorts");
	JSStringRef ConnectJ = JSStringCreateWithUTF8CString("Connect");
	JSStringRef RequestJ = JSStringCreateWithUTF8CString("Requests");
    // Create a garbage-collected JavaScript function that is bound to our
    // native C callback 'OnButtonClick()'.
    JSObjectRef LoadPJFunc = JSObjectMakeFunctionWithCallback(ctx,LoadPJ, LoadPorts);
	JSObjectRef ConnectJFunc = JSObjectMakeFunctionWithCallback(ctx, ConnectJ, Connect);
	JSObjectRef RequestJFunc = JSObjectMakeFunctionWithCallback(ctx, RequestJ, Requests);

    // Get the global JavaScript object (aka 'window')
    JSObjectRef globalObj = JSContextGetGlobalObject(ctx);

    // Store our function in the page's global JavaScript object so that it
    // accessible from the page as 'OnButtonClick()'.
    JSObjectSetProperty(ctx, globalObj, LoadPJ, LoadPJFunc, 0, 0);
	JSObjectSetProperty(ctx, globalObj, ConnectJ, ConnectJFunc, 0, 0);
	JSObjectSetProperty(ctx, globalObj, RequestJ, RequestJFunc, 0, 0);

    // Release the JavaScript String we created earlier.
    JSStringRelease(LoadPJ);
	JSStringRelease(ConnectJ);
	JSStringRelease(RequestJ);
}

void MyApp::OnChangeCursor(ultralight::View* caller,
                           Cursor cursor) {
  ///
  /// This is called whenever the page requests to change the cursor.
  ///
  /// We update the main window's cursor here.
  ///
  window_->SetCursor(cursor);
}

void MyApp::OnChangeTitle(ultralight::View* caller,
                          const String& title) {
  ///
  /// This is called whenever the page requests to change the title.
  ///
  /// We update the main window's title here.
  ///
  window_->SetTitle(title.utf8().data());
}





