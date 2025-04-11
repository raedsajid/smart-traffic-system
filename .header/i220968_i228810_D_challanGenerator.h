#pragma once
/*
void challanGeneratorProcess() {
    RenderVehicle vehicle;
    while (read(smartToChallan[0], &vehicle, sizeof(vehicle)) > 0) {
        int fine = (vehicle.type == "Heavy") ? 7000 : 5000;
        fine += static_cast<int>(fine * 0.17); // Add service charge

        Challan challan = {vehicle.numberPlate, fine, "unpaid", "today's date", "due date"};
        write(challanToUser[1], &challan, sizeof(challan));
    }
    // Process payment updates from Stripe Payment
    PaymentStatus status;
    while (read(stripeToChallan[0], &status, sizeof(status)) > 0) {
        // Update challan payment status
    }
}

*/