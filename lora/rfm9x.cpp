/*
 * lora::rfm9x for Raspberry Pi Pico
 *
 * Adapted from
 * https://github.com/adafruit/Adafruit_CircuitPython_RFM9x/blob/main/adafruit_rfm9x.py
 *
 * @version     1.0.0
 * @author      smittytone
 * @copyright   2021
 * @licence     MIT
 *
 */
#include "main.h"


RFM9x::RFM9x(uint8_t reset_pin, double frequency) {
    // Set up the RST pin: set general state to HIGH
    // and trigger a manual reset
    rst_pin = reset_pin;
    gpio_init(rst_pin);
    gpio_set_dir(rst_pin, GPIO_OUT);
    gpio_put(rst_pin, true);
    reset();

    // Check the SX127 version register
    uint32_t version = SPI::read_byte(RH_RF95_REG_42_VERSION);
    if (version != 0x12) {
        boot_err = 1;
        return;
    }

    // Set sleep mode, wait 10ms and set LoRa mode
    // (which MUST be done while in sleep - see datasheet p.108)
    sleep();
    sleep_ms(10);
    long_range_mode.set(1);

    // Do a basic device check: we're in sleep mode and in LoRa
    // mode (not the FSK/OOK mode)
    if (op_mode.get() != SLEEP_MODE || long_range_mode.get() == 0) {
        // ERROR!
        boot_err = long_range_mode.get();
        return;
    }

    // Handle the frequency type. SX127 boot default is that
    // low frequency mode is set
    if (frequency > 525) low_freq_mode.set(0);

    // Setup the entire 256 byte FIFO and go idle
    SPI::write_byte(RH_RF95_REG_0E_FIFO_TX_BASE_ADDR, 0x00);
    SPI::write_byte(RH_RF95_REG_0F_FIFO_RX_BASE_ADDR, 0x00);
    standby();

    // Set the carrier frequency from the arg
    // (MUST be set in standy)
    set_frequency(frequency);
    // Set preamble length to match Radiohead
    set_preamble_length(8);
    // Set signal bandwidth to match RadioHead
    set_signal_bw(125000);
    // Set coding rate to match RadioHead
    set_coding_rate(5);
    // Set spreading factor to match RadioHead
    set_spreading_factor(7);

    // Default to enable CRC checking on incoming packets
    enable_crc(true);

    // Disable Auto Gain Control. Default: off
    auto_agc.set(0);

    // Set transmit power to 13 dBm, a safe value any module supports
    set_tx_power(13);

    // Radio is good
    state = true;
}

/*
    Reset the chip.

    See section 7.2.2 of the datasheet for reset description.
 */
void RFM9x::reset() {
    // Pull RESET LOW...
    gpio_put(rst_pin, false);
    // ...wait >100us...
    sleep_us(110);
    // ...set RESET HIGH again...
    gpio_put(rst_pin, true);
    // ... wait >5s
    sleep_ms(6);
}

/*
    Enter idle standby mode.
 */
void RFM9x::standby() {
    op_mode.set(STANDBY_MODE);
}

/*
    Enter sleep mode.
 */
void RFM9x::sleep() {
    op_mode.set(SLEEP_MODE);
}

/*
    The length of the preamble for sent and received packets, an unsigned
    16-bit value. Received packets must match this length or they are
    ignored! Set to 8 to match the RadioHead RFM95 library.
 */
uint32_t RFM9x::get_preamble_length() {
    uint8_t msb = SPI::read_byte(RH_RF95_REG_20_PREAMBLE_MSB);
    uint8_t lsb = SPI::read_byte(RH_RF95_REG_21_PREAMBLE_LSB);
    return (((msb << 8) | lsb) & 0xFFFF);
}

void RFM9x::set_preamble_length(uint32_t length) {
    if (length > 0xFFFF) length = 0xFFFF;
    SPI::write_byte(RH_RF95_REG_20_PREAMBLE_MSB, ((length & 0xFF00) >> 8));
    SPI::write_byte(RH_RF95_REG_21_PREAMBLE_LSB, (length & 0xFF));
}

/*
    The frequency of the radio in Megahertz. Only the allowed values for
    your radio must be specified (ie. 433 or. 915 mhz)
 */
float RFM9x::get_frequency() {
    uint8_t msb = SPI::read_byte(RH_RF95_REG_06_FRF_MSB);
    uint8_t mid = SPI::read_byte(RH_RF95_REG_07_FRF_MID);
    uint8_t lsb = SPI::read_byte(RH_RF95_REG_08_FRF_LSB);
    uint32_t frf = ((msb << 16) | (mid << 8) | lsb);
    return (float)(frf * RH_RF95_FSTEP) / 1000000.0;
}

void RFM9x::set_frequency(double value) {
    uint32_t frf = int((value * 1000000.0) / RH_RF95_FSTEP) & 0xFFFFFF;
    // Extract byte values and update registers.
    uint8_t msb = frf >> 16;
    uint8_t mid = frf >> 8;
    uint8_t lsb = frf & 0xFF;
    SPI::write_byte(RH_RF95_REG_06_FRF_MSB, msb);
    SPI::write_byte(RH_RF95_REG_07_FRF_MID, mid);
    SPI::write_byte(RH_RF95_REG_08_FRF_LSB, lsb);
}

/*
    The transmit power in dBm. Can be set to a value from 5 to 23 for
    high power devices (RFM95/96/97/98, high_power = true), or -1 to 14 for low
    power devices. Only integer power levels are actually set (ie. 12.5
    will result in a value of 12 dBm).

    The actual maximum setting for high_power=True is 20dBm but for values > 20
    the PA_BOOST will be enabled resulting in an additional gain of 3dBm.
    The actual setting is reduced by 3dBm.

    The reported value will reflect the reduced setting.
 */
uint32_t RFM9x::get_tx_power() {
    if (high_power) return output_power.get() + 5;
    return output_power.get() - 1;
}

void RFM9x::set_tx_power(uint32_t value) {
    if (high_power) {
        if (value < 5 || value > 23) {
            // ERROR
            return;
        }

        if (value > 20) {
            pa_dac.set(RH_RF95_PA_DAC_ENABLE);
            value -= 3;
        } else {
            pa_dac.set(RH_RF95_PA_DAC_DISABLE);
        }
        pa_select.set(1);
        output_power.set((value - 5) & 0xFF);
    } else {
        if (value > 14) {
            // ERROR
            return;
        }

        pa_select.set(0);
        max_power.set(7);
        output_power.set((value + 1) & 0x0F);
    }
}

/*
    The received strength indicator (in dBm) of the last received message.
 */
double RFM9x::get_rssi() {
    int8_t raw_rssi = SPI::read_byte(RH_RF95_REG_1A_PKT_RSSI_VALUE);
    raw_rssi -= (low_freq_mode.get() == 1 ? 157 : 164);
    return (double)raw_rssi;
}

/*
    The SNR (in dB) of the last received message.
 */
double RFM9x::get_snr() {
    int8_t snr_byte = SPI::read_byte(RH_RF95_REG_19_PKT_SNR_VALUE);
    if (snr_byte > 127) snr_byte = (256 - snr_byte) * -1;
    return (double)(snr_byte / 4.0);
}

/*
    The signal bandwidth used by the radio (try setting to a higher
    value to increase throughput or to a lower value to increase the
    likelihood of successfully received payloads). Valid values are
    listed in `bws`.
 */
void RFM9x::set_signal_bw(uint32_t value) {
    uint8_t bw_id = 0;
    for (uint32_t i = 0 ; i < 9 ; ++i) {
        bw_id = (uint8_t)i;
        uint32_t cutoff = bws[i + 1];
        if (value <= cutoff) break;
        bw_id = 9;
    }

    SPI::write_byte(RH_RF95_REG_1D_MODEM_CONFIG1, (SPI::read_byte(RH_RF95_REG_1D_MODEM_CONFIG1) & 0x0F) | (bw_id << 4));

    if (value >= 500000) {
        // see Semtech SX1276 errata note 2.3
        auto_ifon.set(1);
        // see Semtech SX1276 errata note 2.1
        if (low_freq_mode.get() == 1) {
            SPI::write_byte(0x36, 0x02);
            SPI::write_byte(0x3A, 0x7F);
        } else {
            SPI::write_byte(0x36, 0x02);
            SPI::write_byte(0x3A, 0x64);
        }
    } else {
        // see Semtech SX1276 errata note 2.3
        auto_ifon.set(0);
        SPI::write_byte(0x36, 0x03);
        if (value == 7800) {
            SPI::write_byte(0x2F, 0x48);
        } else if (value >= 62500) {
            // see Semtech SX1276 errata note 2.3
            SPI::write_byte(0x2F, 0x40);
        } else {
            SPI::write_byte(0x2F, 0x44);
            SPI::write_byte(0x30, 0);
        }
    }
}

/*
    The coding rate used by the radio to control forward error
    correction (try setting to a higher value to increase tolerance of
    short bursts of interference or to a lower value to increase bit
    rate).

    Valid values are limited to 5, 6, 7, or 8.
 */
uint32_t RFM9x::get_coding_rate(void) {
    uint8_t cr_id = (SPI::read_byte(RH_RF95_REG_1D_MODEM_CONFIG1) & 0x0E) >> 1;
    return (uint32_t)(cr_id + 4);
}

void RFM9x::set_coding_rate(uint32_t rate) {
    if (rate < 5) rate = 5;
    if (rate > 8) rate = 8;
    rate -= 4;
    SPI::write_byte(RH_RF95_REG_1D_MODEM_CONFIG1, (SPI::read_byte(RH_RF95_REG_1D_MODEM_CONFIG1) & 0xF1) | (rate << 1));
}

/*
    The spreading factor used by the radio (try setting to a higher
    value to increase the receiver's ability to distinguish signal from
    noise or to a lower value to increase the data transmission rate).
    Valid values are limited to 6, 7, 8, 9, 10, 11, or 12.
 */
uint32_t RFM9x::get_spreading_factor(void) {
    return (uint32_t)((SPI::read_byte(RH_RF95_REG_1E_MODEM_CONFIG2) & 0xF0) >> 4);
}

void RFM9x::set_spreading_factor(uint32_t factor) {
    if (factor < 6) factor = 6;
    if (factor > 12) factor = 12;

    detection_optimize.set(factor == 6 ? 0x05 : 0x03);

    SPI::write_byte(RH_RF95_DETECTION_THRESHOLD, (factor == 6 ? 0x0C : 0x0A));
    SPI::write_byte(RH_RF95_REG_1E_MODEM_CONFIG2, (SPI::read_byte(RH_RF95_REG_1E_MODEM_CONFIG2) & 0x0F | ((factor << 4) & 0xF0)));
}

/*
    Set to True to enable hardware CRC checking of incoming packets.
    Incoming packets that fail the CRC check are not processed. Set to
    False to disable CRC checking and process all incoming packets.
 */
void RFM9x::enable_crc(bool state) {
    if (state) {
        SPI::write_byte(RH_RF95_REG_1E_MODEM_CONFIG2, (SPI::read_byte(RH_RF95_REG_1E_MODEM_CONFIG2) | 0x04));
    } else {
        SPI::write_byte(RH_RF95_REG_1E_MODEM_CONFIG2, (SPI::read_byte(RH_RF95_REG_1E_MODEM_CONFIG2) & 0xFB));
    }
}

bool RFM9x::is_crc_enabled(void) {
    return ((SPI::read_byte(RH_RF95_REG_1E_MODEM_CONFIG2) & 0x04) == 0x04);
}

/*
    Transmit status.
 */
bool RFM9x::tx_done() {
    return (((SPI::read_byte(RH_RF95_REG_12_IRQ_FLAGS) & 0x8) >> 3) > 0);
}

/*
    Receive status.
 */
bool RFM9x::rx_done() {
    return (((SPI::read_byte(RH_RF95_REG_12_IRQ_FLAGS) & 0x40) >> 6) > 0);
}

/*
    CRC status.
 */
uint32_t RFM9x::crc_error() {
    return (uint32_t)((SPI::read_byte(RH_RF95_REG_12_IRQ_FLAGS) & 0x20) >> 5);
}

bool RFM9x::send(uint8_t* data, uint32_t length, bool keep_listening) {
    // Stop receiving to clear FIFO and keep it clear
    standby();

    // Fill the FIFO with a packet to send
    SPI::write_byte(RH_RF95_REG_0D_FIFO_ADDR_PTR, 0x00);

    // Combine header and data to form payload
    uint8_t payload[256];
    for (uint32_t i = 0 ; i < 256 ; ++i) payload[i] = 0x00;

    payload[0] = destination;
    payload[1] = node;
    payload[2] = identifier;
    payload[3] = flags;

    // Add in the data
    if (length > 256 - HEADER_LENGTH) length = 256 - HEADER_LENGTH;
    for (uint32_t i = 0 ; i < length ; ++i) payload[i + HEADER_LENGTH] = data[i];

    // Write out the payload
    SPI::write_from(RH_RF95_REG_00_FIFO, payload, length + HEADER_LENGTH);

    // Write payload and header length.
    SPI::write_byte(RH_RF95_REG_22_PAYLOAD_LENGTH, length + HEADER_LENGTH);

    // Turn on transmit mode to send out the packet.
    transmit();

    // Wait for tx done interrupt with explicit polling (not ideal but
    // best that can be done right now without interrupts)
    uint32_t start = time_us_32();
    bool timed_out = false;
    while (!timed_out && !tx_done()) {
        if ((time_us_32() - start) >= xmit_timeout) timed_out = true;
    }

    // Listen again if necessary and return the result packet
    if (keep_listening) {
        listen();
    } else {
        // Enter idle mode to stop receiving other packets
        standby();
    }

    // Reset interrupt
    SPI::write_byte(RH_RF95_REG_12_IRQ_FLAGS, 0xFF);
    return !timed_out;
}

/*
    Listen for packets to be received by the chip. Use `receive()`
    to listen, wait and retrieve packets as they're available.
 */
void RFM9x::listen() {
    op_mode.set(RX_MODE);
    // Interrupt on rx done
    dio0_mapping.set(0);
}

/*
    Transmit a packet which is queued in the FIFO. This is a low level
    function for entering transmit mode and more. For generating and
    transmitting a packet of data use `send()` instead.
 */
void RFM9x::transmit() {
    op_mode.set(TX_MODE);
    // Interrupt on tx done
    dio0_mapping.set(1);
}