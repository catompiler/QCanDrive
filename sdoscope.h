#ifndef SDOSCOPE_H
#define SDOSCOPE_H

#include <QObject>
#include <QVector>
#include "cotypes.h"
#include "covaluetypes.h"


class SDOComm;
class SLCanOpenNode;
class RegListModel;
class RegVar;


class SDOScope : public QObject
{
    Q_OBJECT
public:
    explicit SDOScope(QObject *parent = nullptr);
    ~SDOScope();

    const RegListModel* regListModel() const;
    RegListModel* regListModel();
    void setRegListModel(RegListModel* newRegListModel);

    SLCanOpenNode* getSLCanOpenNode();
    void setSLCanOpenNode(SLCanOpenNode* slcon);

    CO::NodeId nodeId() const;
    void setNodeId(CO::NodeId newNodeId);

    CO::Index entryIndex() const;
    void setEntryIndex(CO::Index newEntryIndex);

    CO::SubIndex versionSubIndex() const;
    void setVersionSubIndex(CO::SubIndex newVersionSubIndex);

    //! Версия модуля.
    enum Ver { OSCOPE_VERSION = 0x0001 };

    //! Ограничения.
    enum Constraints {
        MAX_CHANNELS = 64, /* Максимальное число каналов. */
        MAX_SAMPLES = 16384, /* Максимальное число семплов. */
    };

    //! Подындексы по-умолчанию.
    enum SubIndices {
        CONTROL_SUBINDEX = 0x01, /* Слово управления. */
        STATUS_SUBINDEX = 0x02, /* Слово состояния. */
        VERSION_SUBINDEX = 0x03, /* Версия модуля. */
        MAX_CHANNELS_SUBINDEX = 0x04, /* Максимальное число каналов. */
        MAX_SAMPLES_SUBINDEX = 0x05, /* Максимальное число семплов. */
        MAX_SAMPLE_RATE_SUBINDEX = 0x06, /* Частота дискретизации. */
        SAMPLES_SUBINDEX = 0x07, /* Число семплов (длина буфера). */
        PRESCALER_SUBINDEX = 0x08, /* Предделитель. */
        HIST_SAMPLES_SUBINDEX = 0x09, /* Семплы истории (0..SAMPLES-1) */
        CH0_ENABLED_SUBINDEX = 0x0a, /* Флаг разрешения осциллографирования. */
        CH0_REG_ID_SUBINDEX = 0x0b, /* Идентификатор регистра для осциллографирования. */
        CH1_ENABLED_SUBINDEX = 0x0c, /* Флаг разрешения осциллографирования. */
        CH1_REG_ID_SUBINDEX = 0x0d, /* Идентификатор регистра для осциллографирования. */
        CH2_ENABLED_SUBINDEX = 0x0e, /* Флаг разрешения осциллографирования. */
        CH2_REG_ID_SUBINDEX = 0x0f, /* Идентификатор регистра для осциллографирования. */
        CH3_ENABLED_SUBINDEX = 0x10, /* Флаг разрешения осциллографирования. */
        CH3_REG_ID_SUBINDEX = 0x11, /* Идентификатор регистра для осциллографирования. */
        CH4_ENABLED_SUBINDEX = 0x12, /* Флаг разрешения осциллографирования. */
        CH4_REG_ID_SUBINDEX = 0x13, /* Идентификатор регистра для осциллографирования. */
        CH5_ENABLED_SUBINDEX = 0x14, /* Флаг разрешения осциллографирования. */
        CH5_REG_ID_SUBINDEX = 0x15, /* Идентификатор регистра для осциллографирования. */
        CH6_ENABLED_SUBINDEX = 0x16, /* Флаг разрешения осциллографирования. */
        CH6_REG_ID_SUBINDEX = 0x17, /* Идентификатор регистра для осциллографирования. */
        CH7_ENABLED_SUBINDEX = 0x18, /* Флаг разрешения осциллографирования. */
        CH7_REG_ID_SUBINDEX = 0x19, /* Идентификатор регистра для осциллографирования. */
        CH_DATA0_SAMPLES_SUBINDEX = 0x1a, /*  */
        CH_DATA1_SAMPLES_SUBINDEX = 0x1b, /*  */
        CH_DATA2_SAMPLES_SUBINDEX = 0x1c, /*  */
        CH_DATA3_SAMPLES_SUBINDEX = 0x1d, /*  */
        CH_DATA4_SAMPLES_SUBINDEX = 0x1e, /*  */
        CH_DATA5_SAMPLES_SUBINDEX = 0x1f, /*  */
        CH_DATA6_SAMPLES_SUBINDEX = 0x20, /*  */
        CH_DATA7_SAMPLES_SUBINDEX = 0x21, /*  */
        MODE_SUBINDEX = 0x22, /* Режим работы осциллографа. */
        TRIG_ENABLED_SUBINDEX = 0x23, /* Включение триггера. */
        TRIG_CH_N_SUBINDEX = 0x24, /* Номер канала. */
        TRIG_TYPE_SUBINDEX = 0x25, /* Тип триггера. */
        TRIG_VALUE_SUBINDEX = 0x26, /* Значение триггера. */
        START_INDEX_SUBINDEX = 0x27, /* Начальный индекс записи (индекс сработки триггера). */
        CUR_INDEX_SUBINDEX = 0x28, /* Индекс вставки. */
        CUR_COUNT_SUBINDEX = 0x29, /* Текущее число семплов. */
    };

    //! Перечисление управления.
    enum Control {
        CONTROL_NONE = 0x00, //!< Ничего.
        CONTROL_RESET = 0x01, //!< Сброс.
        CONTROL_ENABLE = 0x02, //!< Разрешение работы.
        CONTROL_START = 0x04, //!< Запуск.
        //CONTROL_STOP = 0x08, //!< Останов.
        //CONTROL_USER = 0x10 //!< Управление модулей.
    };

    //! Перечисление состояния.
    enum Status {
        STATUS_NONE = 0x00, //!< Ничего.
        STATUS_READY = 0x01, //!< Готовность.
        STATUS_VALID = 0x02, //!< Правильность выходных данных.
        STATUS_RUN = 0x04, //!< Работа.
        //STATUS_ERROR = 0x08, //!< Ошибка.
        //STATUS_WARNING = 0x10, //!< Предупреждение.
        //STATUS_USER = 0x20 //!< Статусы модулей.
    };

    //! Перечисление ошибок.
    enum Error {
        ERROR_NONE = 0,
        ERROR_COMM = 1,
        ERROR_VERSION = 2,
        ERROR_CONSTRAINTS = 3,
        ERROR_ = 4,
    };

    //! Перечисление активных КА.
    enum State {
        STATE_NONE = 0,
        STATE_INIT,
        STATE_UPDATE,
        STATE_APPLY,
        STATE_RUN,
        STATE_READ
    };

    //! КА инициализации.
    enum InitState {
        INIT_NONE = 0,
        INIT_BEGIN,
        INIT_VERSION,
        INIT_TASKS,
        INIT_DONE
    };

    //! КА обновления.
    enum UpdateState {
        UPDATE_NONE = 0,
        UPDATE_BEGIN,
        UPDATE_TASKS,
        UPDATE_BASE,
        UPDATE_DONE
    };

    //! КА применения настроек.
    enum ApplyState {
        APPLY_NONE = 0,
        APPLY_BEGIN,
        APPLY_COMMON,
        APPLY_TRIG,
        APPLY_CHANNLES,
        APPLY_WAIT,
        APPLY_DONE
    };

    //! КА записи осциллограммы.
    enum RunState {
        RUN_NONE = 0,
        RUN_BEGIN,
        RUN_START,
        RUN_WAIT,
        RUN_DONE
    };

    //! КА чтения данных осциллограммы.
    enum ReadState {
        READ_NONE = 0,
        READ_BEGIN,
        READ_DATA,
        READ_DONE
    };

    // Канал.
    class Channel {
    public:
        friend class SDOScope;

        // Тип семпла.
        typedef int32_t sample_t;

        Channel();
        Channel(const Channel& ch) = delete;
        Channel(Channel&& ch);
        ~Channel();

        /**
         * @brief Получает разрешение осциллографирования канала.
         * Настройка в осциллографе.
         * @return Разрешение осциллографирования канала.
         */
        bool enabled() const;
        void setEnabled(bool newEnabled);

        /**
         * @brief Получает индекс регистра для осциллографирования.
         * Настройка в осциллографе.
         * @return Индекс регистра для осциллографирования.
         */
        CO::Index regIndex() const;
        void setRegIndex(CO::Index newIndex);

        /**
         * @brief Получает подындекс регистра для осциллографирования.
         * Настройка в осциллографе.
         * @return Подындекс регистра для осциллографирования.
         */
        CO::SubIndex regSubIndex() const;
        void setRegSubIndex(CO::SubIndex newSubIndex);

        /**
         * @brief Число семплов канала.
         * @return Число семплов канала.
         */
        uint samplesCount() const;

        /**
         * @brief Получает значение i-го семпла канала.
         * @param i Индекс семпла.
         * @return Значение i-го семпла канала.
         */
        sample_t sample(uint i) const;
        bool setSample(uint i, sample_t value);

        /**
         * @brief Получает тип данных регистра для осциллографирования.
         * @return Тип данных регистра для осциллографирования.
         */
        DataType dataType() const;
        void setDataType(DataType newDataType);

        /**
         * @brief Получает базовое значение для значения осциллографируемого регистра.
         * @return Базовое значение для значения осциллографируемого регистра.
         */
        qreal baseValue() const;
        void setBaseValue(qreal newBaseValue);

        /**
         * @brief Получает сырое значение величины.
         * @return Сырое значение величины.
         */
        qreal rawValue(uint i) const;

        /**
         * @brief Получает значение величины с учётом базового значения.
         * @return Значение величины.
         */
        qreal value(uint i) const;

    protected:
        //! Получает указатель на разрешение осциллографирования канала.
        uint32_t* enabledPtr();
        const uint32_t* enabledPtr() const;

        //! Получает указатель на идентификатор регистра для осциллографирования канала.
        uint32_t* regIdPtr();
        const uint32_t* regIdPtr() const;

        //! Получает указатель на массив семплов канала.
        void* samplesPtr();
        const void* samplesPtr() const;

        // Вызывает класс осциллографа после чтения конфигурации.
        // Изменяет размер буфера с семплами.
        void resize(uint newSize);

    private:
        uint32_t m_enabled; //!< Включение канала.
        uint32_t m_reg_id; //!< Идентификатор регистра.
        sample_t* m_samples; //!< Массив семплов.
        uint m_samples_count; //!< Число семплов в массиве.
        // Параметры осциллографируемого регистра.
        // Осциллографируемый регистр.
        DataType m_dataType;
        // Базовый регистр.
        // Базовое значение.
        qreal m_base_value;
    };

    //! Получить версию осциллографа.
    uint version() const;

    //! Получить максимальное количество каналов.
    uint maxChannelsCount() const;

    //! Получить максимальное число семплов.
    uint maxSamplesCount() const;

    //! Получить частоту дискретизации.
    uint32_t maxSampleRate() const;

    //! Получить ошибку.
    Error error() const;

    //! Проверить, инициализирован ли осциллограф.
    bool isInitialized() const;

    //! Проверить, обновлены ли данные осциллографа.
    bool isUpdated() const;

    //! Получить число каналов.
    uint channelsCount() const;

    //! Получить число семплов.
    uint samplesCount() const;

    //! Получить частоту дискретизации.
    uint sampleRate() const;

    //! Получить семплы истории.
    uint histSamplesCount() const;

    //! Получить режим работы.
    uint mode() const;

    //! Получить канал.
    Channel* channel(uint i);

    //! Получить канал.
    const Channel* channel(uint i) const;

public slots:
    bool init();
    bool deinit();
    bool update();
    bool apply();
    bool run();
    bool read();

signals:
    void errorOccured(); // Сигнал о возникновении ошибки.
    void initialized(); // Сигнал об окончании инициализации.
    void updated(); // Сигнал об окончании обновления.
    void applied(); // Сигнал об окончании применения настроек.
    void done(); // Сигнал об окончании записи осциллограммы.
    void readed(); // Сигнал об окончании чтения данных осциллограммы.
    void finished(); // Сигнал об окончании любой операции.

private slots:
    void sdoFinished();

private:
    RegListModel* m_regListModel;

    // Структура задачи обмена данными.
    struct CommTask {
        enum Dir {READ = 0, WRITE = 1};

//        CommTask(Dir d, CO::SubIndex i, void* p, size_t s){
//            dir = d; index = i; ptr = p; size = s;
//        }

        Dir dir;
        CO::SubIndex index;
        void* ptr;
        size_t size;
    };

    using TasksVector = QVector<CommTask>;

    enum ProcessingState {
        PROCESSING_ERROR = -1,
        PROCESSING_IN_PROGRES = 0,
        PROCESSING_DONE = 1
    };

    SLCanOpenNode* m_slcon;
    CO::NodeId m_nodeId;
    CO::Index m_entryIndex;
    CO::SubIndex m_versionSubIndex;

    SDOComm* m_comm;

    Channel* m_channels;

    Error m_error;
    State m_state;


    // Переменные состояния инициализации.
    InitState m_init_state;
    TasksVector m_init_tasks;
    uint m_init_cur_task;

    // Переменные состояния обновления.
    UpdateState m_update_state;
    TasksVector m_update_tasks;
    uint m_update_cur_task;
    uint m_update_base_ch;
    bool m_update_base_read;
    DataType m_update_base_dataType;
    uint32_t m_update_base_value;

    // Переменные состояния применения.
    ApplyState m_apply_state;
    TasksVector m_apply_common_tasks;
    TasksVector m_apply_trig_tasks;
    TasksVector m_apply_channels_tasks;
    uint m_apply_cur_task;
    bool m_apply_status_read;

    // Переменные состояния записи осциллограммы.
    RunState m_run_state;
    bool m_run_status_read;

    // Переменные состояния чтения данных осциллограммы.
    ReadState m_read_state;
    bool m_read_read;
    uint m_read_ch;


    // Основные поля модуля осциллографа.
    // Поле управления.
    uint32_t m_control;
    // Поле состояния.
    uint32_t m_status;

    // Версия осциллографа на устройстве.
    uint32_t m_version;
    // Максимум каналов на устройстве.
    uint32_t m_max_channels;
    // Максимум семплов канала на устройстве.
    uint32_t m_max_samples;
    // Частота дискретизации.
    uint32_t m_max_sample_rate;

    // Общие параметры.
    // Число семплов.
    uint32_t m_samples;
    // Предделитель.
    uint32_t m_prescaler;
    // Семплы истории.
    uint32_t m_hist_samples;
    // Режим работы.
    uint32_t m_mode;

    // Параметры триггера.
    // Включение.
    uint32_t m_trig_enabled;
    // Номер канала.
    uint32_t m_trig_ch_n;
    // Тип.
    uint32_t m_trig_type;
    // Значение.
    uint32_t m_trig_value;

    // Обработка КА инициализации.
    ProcessingState processInit();
    void populateInitTasks();

    // Обработка КА обновления.
    ProcessingState processUpdate();
    void populateUpdateTasks();

    // Обработка КА применения настроек.
    ProcessingState processApply();
    void populateApplyTasks();

    // Обработка КА записи осциллограммы.
    ProcessingState processRun();

    // Обработка КА чтения данных осциллограммы.
    ProcessingState processRead();

    /*
     * Вызывает сигналы
     * errorOccured() и finished().
     */
    void handleError();
    /*
     * Вызывает сигнал finished().
     */
    void handleFinished();

    bool runCommTask(CommTask* ct);

    /*
     * Получает регистр по индексу и подындексу.
     */
    RegVar* findRegVar(CO::Index rvIndex, CO::SubIndex rvSubIndex);
};

#endif // SDOSCOPE_H
