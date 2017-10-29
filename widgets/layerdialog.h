#ifndef LAYERDIALOG_H
#define LAYERDIALOG_H

#include "widgets/DialogVisibilityNotify.h"
#include "widgets/Spoiler.h"

#include <QGroupBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QSlider>
#include <QCheckBox>
#include <QTreeView>
#include <QAbstractItemModel>
#include <QMessageBox>
#include <QListWidget>

class LayerLoadWidget : public QDialog {
Q_OBJECT
public:
    LayerLoadWidget(QWidget * parent = 0);

    QVBoxLayout mainLayout;
    QGridLayout listLayout;
    QHBoxLayout buttonLayout;

    QToolButton loadButton;
    QToolButton cancelButton;

    QListWidget datasetLoadList;
    QLabel datasetLoadLabel{"datasetLoadLabel"};
    QLabel datasetLoadDescription{"datasetLoadDescription"};
    QListWidget sessionLayerList;
    QLabel sessionLayerLabel{"layerLoadLabel"};
    QLabel sessionLayerDescription{"sessionLayerDescription"};
};

class LayerItemModel : public QAbstractListModel {
Q_OBJECT
protected:
    const std::vector<QString> header{"visible", "opacity", "name", "cubetype", "api", "mag", "edgelength", "experiment"};
public:
    virtual int rowCount(const QModelIndex & parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex & parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

    void addItem();
    void removeItem(const QModelIndex &index);
    void moveItem(const QModelIndex &indices, int offset);
    void reset();
};

class LayerDialogWidget : public DialogVisibilityNotify {
    Q_OBJECT
public:
    explicit LayerDialogWidget(QWidget * parent = 0);

    QVBoxLayout mainLayout;

    Spoiler optionsSpoiler{"layer options"};
    QGridLayout optionsLayout;
    QLabel opacitySliderLabel{"opacity"};
    QSlider opacitySlider{Qt::Horizontal};
    QLabel rangeDeltaSliderLabel{"range delta"};
    QSlider rangeDeltaSlider{Qt::Horizontal};
    QLabel biasSliderLabel{"bias"};
    QSlider biasSlider{Qt::Horizontal};
    QCheckBox linearFilteringCheckBox{"linear filtering"};

    QVBoxLayout layerLayout;
    LayerItemModel itemModel;
    QTreeView treeView;

    QHBoxLayout controlButtonLayout;
    QLabel infoTextLabel{"Hint: use layers to have layers."};
    QToolButton moveUpButton;
    QToolButton moveDownButton;
    QToolButton addLayerButton;
    QToolButton removeLayerButton;

    LayerLoadWidget layerLoadWidget;

private:
    void updateLayerProperties();
};

#endif // LAYERDIALOG_H
