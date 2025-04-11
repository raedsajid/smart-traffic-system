#pragma once
/*
void stripePaymentProcess() {
    PaymentRequest payment;
    while (read(stripeToUser[0], &payment, sizeof(payment)) > 0) {
        std::cout << "Processing Payment for Challan ID " << payment.id << "\n";
        PaymentStatus status = { payment.id, "paid" };
        write(stripeToChallan[1], &status, sizeof(status));
        write(stripeToUser[1], &status, sizeof(status));
    }
}
*/