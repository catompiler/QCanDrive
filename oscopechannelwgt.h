#ifndef OSCOPECHANNELWGT_H
#define OSCOPECHANNELWGT_H

#include <QWidget>


class OScopePlot;

namespace Ui {
class OScopeChannelWgt;
}

class OScopeChannelWgt : public QWidget
{
    Q_OBJECT

public:
    explicit OScopeChannelWgt(QWidget *parent = nullptr);
    ~OScopeChannelWgt();

    OScopePlot* plot() const;
    void setPlot(OScopePlot* newPlot);

    int channel() const;
    void setChannel(int newChannel);

    bool signalVisible() const;
    void setSignalVisible(bool newVisible);

    QColor penColor() const;
    void setPenColor(const QColor& newPenColor);

    Qt::PenStyle penStyle() const;
    void setPenStyle(Qt::PenStyle newPenStyle);

    qreal penWidth() const;
    void setPenWidth(qreal newPenWidth);

    // Получает значения из графика.
    void updateValues();
    // Применяет значения к графику.
    void applyValues() const;

private slots:
    void vertScaleChanged(qreal value);
    void vertOffsetChanged(qreal value);

    void visiblityCheckStateChanged(int checkState);
    void penColorSel_clicked(bool checked);
    void penStyle_currentIndexChanged(int index);
    void lineWidth_valueChanged(double value);

private:
    Ui::OScopeChannelWgt *ui;

    OScopePlot* m_plot;
    int m_channel;

    void populatePenStyles();
};

#endif // OSCOPECHANNELWGT_H
