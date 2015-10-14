#include "OpenGLWindow.h"

#include <include/Engine.h>
#include <QtGui/QOpenGLContext>
#include <QtPlatformHeaders/QWGLNativeContext>
#include <QtGui/QOffscreenSurface>

#include <include/Qt2Glfw.h>

#include <QKeyEvent>
#include <QWidget>


OpenGLWindow::OpenGLWindow()
{
	setSurfaceType(QSurface::OpenGLSurface);
	container = QWidget::createWindowContainer(this);

	///////////////////////////////////////////////////////////////////////////
	// Window Properties

	container->setMaximumWidth(Engine::Window->resolution.x);
	container->setFixedHeight(Engine::Window->resolution.y);
	Qt2Glfw::Init();
}

OpenGLWindow::~OpenGLWindow()
{
}

void OpenGLWindow::Init()
{
	// OpenGL
	//auto surface = new QOffscreenSurface();
	//surface->setFormat(format);
	//surface->create();
	//setSwapInterval

	QSurfaceFormat format;
	format.setVersion(4, 0);
	format.setSwapInterval(0);
	format.setProfile(QSurfaceFormat::CoreProfile);
	setFormat(format);

	auto glfwCTX = glfwGetWGLContext(Engine::Window->window);
	auto window = glfwGetWin32Window(Engine::Window->window);

	auto newNative = new QWGLNativeContext(glfwCTX, window);
	QOpenGLContext* qtctx = new QOpenGLContext();
	//qtctx->setFormat(format);
	qtctx->setNativeHandle(QVariant::fromValue<QWGLNativeContext>(*newNative));
	bool created = qtctx->create();
	bool value = qtctx->makeCurrent(this);

	// Test
	auto nativeHandle = qtctx->nativeHandle();
	auto windowsNative = nativeHandle.value<QWGLNativeContext>();

	// Share context
	qtGLContext = new QOpenGLContext(this);
	//qtGLContext->setFormat(format);
	qtGLContext->setShareContext(qtctx);
	bool success = qtGLContext->create();

	nativeHandle = qtGLContext->nativeHandle();
	windowsNative = nativeHandle.value<QWGLNativeContext>();

	bool sharing = qtGLContext->areSharing(qtctx, qtGLContext);
	bool sharing2 = qtGLContext->shareContext();
}

void OpenGLWindow::EndFrame()
{
	qtGLContext->swapBuffers(this);
}

QOpenGLContext * OpenGLWindow::GetContext()
{
	return qtGLContext;
}

bool OpenGLWindow::SetAsCurrentContext()
{
	return qtGLContext->makeCurrent(this);
}

void OpenGLWindow::CenterCursor()
{
	skipClipFrame = true;
	cursor().setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void OpenGLWindow::HideCursor()
{
	skipClipFrame = true;
	cursorClip = true;
	QCursor c = cursor();
	c.setShape(Qt::BlankCursor);
	setCursor(c);
}

void OpenGLWindow::ShowCursor()
{
	cursorClip = false;
	QCursor c = cursor();
	c.setShape(Qt::ArrowCursor);
	setCursor(c);
}

void OpenGLWindow::KeyEvent(QKeyEvent * e, bool state)
{
	if (!focused) return;

	int mods = Qt2Glfw::GetModifiers();
	int key = Qt2Glfw::GetKeyButton(e->key());
	InputSystem::KeyCallback(nullptr, key, GLFW_KEY_UNKNOWN, state, mods);

	//cout << "[QT] " << e->key() << "\t[GLFW] " << key << "\t[MODS] " << mods << endl;
}

void OpenGLWindow::MouseEvent(QMouseEvent * e, bool state)
{
	if (!focused) return;
	int mods = Qt2Glfw::GetModifiers();
	int glfwButton = Qt2Glfw::GetMouseButton(e->button());
	int qtButton = e->button();

	prevMousePos = e->windowPos();
	InputSystem::MouseClick(glfwButton, state, e->windowPos().x(), e->windowPos().y(), mods);

	//cout << qtButton << "[" << e->windowPos().x() << ", " << e->windowPos().y() << "]" << endl;
}

void OpenGLWindow::keyPressEvent(QKeyEvent * e)
{
	KeyEvent(e, GLFW_PRESS);
}

void OpenGLWindow::keyReleaseEvent(QKeyEvent * e)
{
	KeyEvent(e, GLFW_RELEASE);
}

void OpenGLWindow::mouseMoveEvent(QMouseEvent * e)
{
	if (!focused) return;
	int deltaX, deltaY;

	if (cursorClip)
	{
		if (skipClipFrame) {
			skipClipFrame = false;
			return;
		}
		auto center = glm::ivec2(width() / 2, height() / 2);
		deltaX = e->windowPos().x() - center.x;
		deltaY = e->windowPos().y() - center.y;
	}
	else {
		deltaX = e->windowPos().x() - prevMousePos.x();
		deltaY = e->windowPos().y() - prevMousePos.y();
	}

	prevMousePos = e->windowPos();
	InputSystem::MouseMove(e->windowPos().x(), e->windowPos().y(), deltaX, deltaY);
	
	if (cursorClip) {
		CenterCursor();
	}
	//cout << "[" << e->windowPos().x() << ", " << e->windowPos().y() << "] delta [" << deltaX << ", " << deltaY << "]" << endl;
}

void OpenGLWindow::mousePressEvent(QMouseEvent * e)
{
	if (!focused) return;

	if (e->button() == Qt::MouseButton::RightButton) {
		HideCursor();
		CenterCursor();
	}

	MouseEvent(e, GLFW_PRESS);
}

void OpenGLWindow::mouseReleaseEvent(QMouseEvent * e)
{
	if (!focused) return;

	if (e->button() == Qt::MouseButton::RightButton) {
		ShowCursor();
	}

	MouseEvent(e, GLFW_RELEASE);
}

void OpenGLWindow::wheelEvent(QWheelEvent * e)
{
	if (!focused) return;
}

void OpenGLWindow::focusInEvent(QFocusEvent * e)
{
	focused = true;
	cout << "[Window] Focus In" << endl;
}

void OpenGLWindow::focusOutEvent(QFocusEvent * e)
{
	focused = false;
	cout << "[Window] Focus Out" << endl;
	ShowCursor();
}

void OpenGLWindow::resizeEvent(QResizeEvent * e)
{
	//cout << "[Window] [Resize] " << e->size().width() << ", "<< e->size().height() << endl;
}
