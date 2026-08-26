#ifndef OSCOPEPLOTSERIESDATA_H
#define OSCOPEPLOTSERIESDATA_H


#include <QwtSeriesData>
#include <QPointF>
#include "oscopevertical.h"

class OScopeData;
class OScopeHorizontal;


class OScopePlotSeriesData : public QwtSeriesData<QPointF>
{
public:
    //! Вызывает update().
    explicit OScopePlotSeriesData(OScopeHorizontal* newHori, OScopeData* newOScopeData = nullptr, uint new_ch_n = 0);
    ~OScopePlotSeriesData();

    /**
     * @brief Устанавливает горизонтальную равзёртку.
     * @param newHori Горизонтальная развёртка.
     */
    void setHorizontal(OScopeHorizontal* newHori);

    /**
     * @brief Получает данные осциллографа.
     * @return Данные осциллографа.
     */
    OScopeData* oscopeData();
    const OScopeData* oscopeData() const;

    /**
     * @brief Устанавливает данные осциллографа.
     * @param newOScopeData Данные осциллографа.
     * После всех установок необходимо вызвать update.
     */
    void setOScopeData(OScopeData* newOScopeData);

    /**
     * @brief Устанавливает номер канала.
     * @param new_ch_n Номер канала.
     * После всех установок необходимо вызвать update.
     */
    void setChannel(uint new_ch_n);

    // Вертикальная развёртка, вольт / деление.
    qreal vDiv() const;
    void setVDiv(qreal newVDiv);
    qreal invVDiv() const;

    // Вертикальная развёртка, смещение.
    qreal vOffset() const;
    void setVOffset(qreal newVOffset);

    /**
     * @brief Получает время точки графика
     * без преобразований (согласно данным).
     * @param idx Индекс семпла.
     * @return Значение время точки по индексу.
     */
    qreal time(size_t idx) const;

    /**
     * @brief Получает значение графика
     * без преобразований (согласно данным).
     * @param idx Индекс семпла.
     * @return Значение графика по индексу.
     */
    qreal value(size_t idx) const;

    /**
     * @brief Сбрасывает кэшированный ограничивающий прямоугольник.
     */
    void invalidateBounds();

    // /**
    //  * @brief Обновляет внутренние данные.
    //  * Обновляет bounding rect и другие данные.
    //  */
    // void update();

    //! Число точек.
    size_t size() const override;
    //! Получает точку.
    QPointF sample(size_t i) const override;
    //! Получает огрничивающий прямоугольник.
    QRectF boundingRect() const override;

private:
    OScopeHorizontal* m_hori;
    OScopeVertical* m_vert;
    OScopeData* m_oscData;
    uint m_ch_n;
    //! Обновляет границы.
    void updateBounds() const;
    //! Получает значение семпла по-вертикали.
    qreal getYValue(size_t i) const;
    //! Получает значение семпла по-горизонтали.
    qreal getXValue(size_t i) const;
};

#endif // OSCOPEPLOTSERIESDATA_H
