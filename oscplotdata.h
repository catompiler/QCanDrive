#ifndef OSCPLOTDATA_H
#define OSCPLOTDATA_H


#include <QwtSeriesData>
#include <QPointF>

class SDOScope;


class OscPlotData : public QwtSeriesData<QPointF>
{
public:
    //! Вызывает update().
    explicit OscPlotData(SDOScope* newScope = nullptr, uint new_ch_n = 0);
    ~OscPlotData();

    size_t size() const override;

    QPointF sample(size_t i) const override;
    QRectF boundingRect() const override;

    /**
     * @brief Устанавливает осциллограф.
     * @param newScope Осциллограф.
     * После всех установок необходимо вызвать update.
     */
    void setScope(SDOScope* newScope);

    /**
     * @brief Устанавливает номер канала.
     * @param new_ch_n Номер канала.
     * После всех установок необходимо вызвать update.
     */
    void setChannel(uint new_ch_n);

    // Смещение данных.
    qreal offset() const;
    void setOffset(qreal val);

    // Усиление данных.
    qreal gain() const;
    void setGain(qreal val);

    /**
     * @brief Обновляет внутренние данные.
     * Обновляет bounding rect и другие данные.
     */
    void update();

private:
    SDOScope* m_scope;
    uint m_ch_n;

    qreal m_offset;
    qreal m_gain;

    qreal m_Ts;
    QRectF m_bounds;

    void updateTs();
    void updateBounds();

    qreal getValue(size_t i) const;
};

#endif // OSCPLOTDATA_H
