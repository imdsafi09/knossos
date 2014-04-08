#ifndef SKELETONPROXY_H
#define SKELETONPROXY_H

#include <QObject>
#include <QList>
#include "knossos-global.h"


/** Actually this class it not really needed. It only hides the SIGNALS from the SkeletonProxy */
class SkeletonProxySignalDelegate : public QObject  {
    Q_OBJECT
signals:
    bool loadSkeleton(QString file);
    bool saveSkeleton(QString file);
    bool addNodeSignal(Coordinate *coordinate, Byte VPtype);
    void treeAddedSignal(TreeListElement *tree);
    void nodeAddedSignal();
    void updateToolsSignal();
    void clearSkeletonSignal();
    void userMoveSignal(int x, int y, int z);
    void echo(QString message);
    void updateTreeViewSignal();
    bool sliceExtractSignal(Byte *datacube, Byte *slice, vpConfig *vpConfig);
    void saveSettingsSignal(const QString &key, const QVariant &value);
    uint renderTextSignal(Coordinate *pos, char *string, uint currentVP, uint viewportType);
};

extern SkeletonProxySignalDelegate *signalDelegate;

class SkeletonProxy : public QObject
{
    Q_OBJECT
public:
    explicit SkeletonProxy(QObject *parent = 0);

signals:

public slots:
    int skeleton_time();
    QString skeleton_file();
    void to_xml(const QString &filename);
    void from_xml(const QString &filename);
    TreeListElement *first_tree();
    bool has_unsaved_changes();
    void delete_tree(int tree_id);
    void delete_skeleton();
    void set_active_node(int node_id);
    void add_comment(int node_id, char *comment);
    NodeListElement *active_node();
    QList<TreeListElement *> *trees();
    void add_tree(int tree_id, const QString &comment = 0, float r = -1, float g = -1, float b = -1, float a = 1);
    void add_node(int node_id, int x, int y, int z, int parent_tree_id = 0, float radius = 1.5, int inVp = 0, int inMag = 1, int time = 0);
    void add_segment(int source_id, int target_id);
    void add_branch_node(int node_id);
    QList<int> *cube_data_at(int x, int y, int z);
    void render_mesh(Mesh *Mesh);

    void export_converter(const QString &path);
    void save_sys_path(const QString &path);
    void save_working_directory(const QString &path);

    void loadStyleSheet(const QString &path);
    QList<Mesh *> *user_geom_list();
    void move_to(int x, int y, int z);
    void render_text(const QString &path, int x, int y, int z);

    static QString help();
};

#endif // SKELETONPROXY_H
