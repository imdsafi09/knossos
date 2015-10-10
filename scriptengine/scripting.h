#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <functional>
#include <QMouseEvent>
#include <QObject>
#include <QThread>
#include <PythonQt/PythonQt.h>
#ifdef QtAll
#include <PythonQt/PythonQt_QtAll.h>
#endif
#include <PythonQt/PythonQtClassInfo.h>
#include <PythonQt/PythonQtPythonInclude.h>
#include <PythonQt/PythonQtStdIn.h>

#include "stateInfo.h"
#include "widgets/viewport.h"

class ColorDecorator;
class FloatCoordinateDecorator;
class CoordinateDecorator;
class TreeListDecorator;
class NodeListDecorator;
class NodeCommentDecorator;
class SegmentListDecorator;
class MeshDecorator;
class SegmentationProxy;
class SkeletonProxy;
class PythonProxy;
class TransformDecorator;
class PointDecorator;
class Highlighter;
class QSettings;
class PythonQtObjectPtr;

void PythonQtInit();

/*
 * This class emits on ctor a signal of arbitrary parameters that is passed to the ctor,
 * and on dtor a dtorSignal with a context parameter that may be set by dtorSetCtx.
 * Instantiate within a C++ scope to allow PythonQt python code to be notified when the
 * scope begins, and then when the scope terminates (upon completion/exception/break/return/etc.).
 *
 * Typical PythonQt usage:
 *
 * def mySignal_dtor_func(ctx):
 *      print "dtor: %s\n" % ctx
 *      return
 *
 * def mySignal_ctor_func(eocd, a, b, c):
 *      eocd.connect(mySignal_dtor_func)
 *      description = "%s: %d, %d" % (a, b, c)
 *      eocd.dtorSetCtx(description)
 *      print "mySignal_ctor: %s\n" % description
 *      return
 *
 * signalRelay.mySignal.connect(mySignal_ctor_func)
 *
*/
class EmitOnCtorDtor : public QObject
{
    Q_OBJECT
public:
    template<typename SignalType, typename Class, typename... Args>
    EmitOnCtorDtor(SignalType signalName, Class instance, Args && ...args) {
        emit std::mem_fn(signalName)(instance, this, std::forward<Args>(args)...);
    }
    ~EmitOnCtorDtor() {
        emit dtorSignal(dtorCtx);
    }
public slots:
    void dtorSetCtx(QVariant ctx) { dtorCtx = ctx;}
signals:
    void dtorSignal(QVariant);
private:
    QVariant dtorCtx;
};

class SignalRelay : public QObject
{
    Q_OBJECT
public:
    explicit SignalRelay() {state->signalRelay = this;}
signals:
    void Signal_EventModel_handleMouseHover(EmitOnCtorDtor*,Coordinate,quint64,int,QMouseEvent*);
    void Signal_Viewort_mouseReleaseEvent(EmitOnCtorDtor*,class ViewportBase*,QMouseEvent*);
    void Signal_EventModel_handleMouseReleaseMiddle(EmitOnCtorDtor*,Coordinate,int,QMouseEvent*);
    void Signal_MainWindow_closeEvent(EmitOnCtorDtor*,QCloseEvent*);
};

/** This class intializes the python qt engine */
class Scripting : public QObject
{
    Q_OBJECT
public:
    explicit Scripting();
    void runFile(const QString &filename);
    void addObject(const QString& name, QObject* object);
    void addVariable(const QString& name, const QVariant& v);
    void importModule(const QString &moduleFullPath);
    CoordinateDecorator *coordinateDecorator;
    FloatCoordinateDecorator *floatCoordinateDecorator;
    ColorDecorator *colorDecorator;
    TreeListDecorator *treeListDecorator;
    NodeListDecorator *nodeListDecorator;
    NodeCommentDecorator *nodeCommentDecorator;
    SegmentListDecorator *segmentListDecorator;
    MeshDecorator *meshDecorator;
    SkeletonProxy *skeletonProxy;
    SegmentationProxy *segmentationProxy;
    PythonProxy *pythonProxy;
    TransformDecorator *transformDecorator;
    PointDecorator *pointDecorator;
    Highlighter *highlighter;
protected:
    QSettings *settings;
private:
    PythonQtObjectPtr _ctx;
    QStringList _customPathDirs;
    void moveSymbolIntoKnossosModule(const QString& name);
    void executeFromUserDirectory();
    void executeResourceStartup();
    void changeWorkingDirectory();
    void addPresetCustomPythonPaths();
    void addCustomPythonPath(const QString &customPath);
    void addWidgets();
    void autoStartTerminal();
};

#endif // SCRIPTING_H
