#pragma once
/*
void userPortalProcess() {
    Challan challan;
    while (read(challanToUser[0], &challan, sizeof(challan)) > 0) {
        std::cout << "Challan for Vehicle " << challan.vehicleNumber
            << " Amount: " << challan.amount << "\n";

        // Initiate payment via StripePayment
        PaymentRequest payment = { challan.id, challan.amount };
        write(stripeToUser[1], &payment, sizeof(payment));
    }
}
*/