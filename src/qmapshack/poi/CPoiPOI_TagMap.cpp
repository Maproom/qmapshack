/**********************************************************************************************
    Copyright (C) 2021 Henri Hornburg <pingurus@t-online.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY); without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

**********************************************************************************************/

#include "poi/CPoiIconCategory.h"
#include "poi/CPoiPOI.h"

QMap<QString, CPoiIconCategory> CPoiPOI::tagMap;
QMap<QString, CPoiIconCategory> CPoiPOI::initTagMap()
{
    QMap<QString, CPoiIconCategory> map;
    /*
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_youth_hostel.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_public_building.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/education_college_vocational.n.32.png"), "School");
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/education_school_primary.n.32.png"), "School");
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/education_school_secondary.n.32.png"), "School");
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_grass.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_hills.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_point_of_interest.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_station_coal.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_station_gas.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_station_solar.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_station_water.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_station_wind.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_substation.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_tower_high.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_tower_low.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/power_transformer.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_hillclimbing.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_helicopter.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_miniroundabout_anticlockwise.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_miniroundabout_clockwise.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_parking_disabled.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_parking.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_parking_private.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_roundabout_anticlockwise.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_roundabout_clockwise.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_speedbump.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_traffic_lights.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_turning_circle.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_walking.n.32.png"));
       map["MISSING"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_zebra_crossing.n.32.png"));
     */
    map["aeroway=aerodrome"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_aerodrome.n.32.png"), "Airport");
    map["aeroway=airport"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_airport.n.32.png"), "Airport");
    map["aeroway=gate"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_airport_gate.n.32.png"), "Airport");
    map["aeroway=terminal"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_airport_terminal.n.32.png"), "Airport");
    map["aeroway=helipad"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_helicopter_pad.n.32.png"), "Heliport");
    map["amenity=atm"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/money_atm.n.32.png"), "Bank");
    map["amenity=bank"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/money_bank.n.32.png"), "Bank");
    map["amenity=bar"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_bar.n.32.png"), "Bar");
    map["amenity=beer_garden'"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_biergarten.n.32.png"));
    map["amenity=bench"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_bench.n.32.png"));
    map["amenity=bicycle_parking"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_parking_bicycle.n.32.png"));
    map["amenity=bicycle_rental"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_rental_bicycle.n.32.png"));
    map["amenity=biergarten"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_biergarten.n.32.png"), "Bar");
    map["amenity=bureau_de_change"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/money_currency_exchange.n.32.png"));
    map["amenity=bus_station"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_bus_station.n.32.png"));
    map["amenity=cafe"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_cafe.n.32.png"), "Restaurant");
    map["amenity=car_rental"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_rental_car.n.32.png"), "Car Rental");
    map["amenity=car_sharing"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_car_share.n.32.png"));
    map["amenity=casino"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_casino.n.32.png"));
    map["amenity=cinema"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_cinema.n.32.png"));
    map["amenity=clock"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_clock.n.32.png"));
    map["amenity=clinic"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/health_hospital.n.32.png"));
    map["amenity=college"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/education_college.n.32.png"), "School");
    map["amenity=courthouse"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_court.n.32.png"), "Scale");
    map["amenity=court_house"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_court.n.32.png"), "Scale");
    map["amenity=dentist"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/health_dentist.n.32.png"), "Medical Facility");
    map["amenity=doctors"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/health_doctors.n.32.png"), "Medical Facility");
    map["amenity=drinking_water"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_drinkingtap.n.32.png"), "Drinking Water");
    map["amenity=embassy"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_embassy.n.32.png"));
    map["amenity=emergency_phone"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_emergency_phone.n.32.png"));
    map["amenity=fast_food"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_fastfood.n.32.png"), "Fast Food",
    {
        {"cuisine=pizza", QPixmap("://icons/poi/SJJB/png/food_fastfood_pizza.n.32.png")}
    });
    map["amenity=fire_station"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_firestation.n.32.png"));
    map["amenity=fountain"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_fountain.n.32.png"));
    map["amenity=fountain"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_fountain.n.32.png"));
    map["amenity=fuel"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_fuel.n.32.png"),
    {
        {"fuel:lpg=yes", QPixmap("://icons/poi/SJJB/png/transport_fuel_lpg.n.32.png")}
    });
    map["amenity=hospital"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/health_hospital.n.32.png"), "Medical Facility",
    {
        {"emergency=yes", QPixmap("://icons/poi/SJJB/png/health_hospital_emergency.n.32.png")}
    });
    map["amenity=ice_cream"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_ice_cream.n.32.png"), "Fast Food");
    map["amenity=kindergarten"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/education_nursery.n.32.png"), "School");
    map["amenity=library"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_library.n.32.png"), "Library");
    map["amenity=marketplace"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_marketplace.n.32.png"));
    map["amenity=parking"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_parking_car.n.32.png"), "Parking Area",
    {
        {"fee=yes", QPixmap("://icons/poi/SJJB/png/transport_parking_car_paid.n.32.png")}
    });
    map["amenity=pharmacy"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/health_pharmacy.n.32.png"), "Pharmacy",
    {
        {"dispensing=yes", QPixmap("://icons/poi/SJJB/png/health_pharmacy_dispencing.n.32.png")}
    });
    map["amenity=place_of_worship"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/place_of_worship_unknown.n.32.png"), "Church",
    {
        {"religion=bahai", QPixmap("://icons/poi/SJJB/png/place_of_worship_bahai.n.32.png")},
        {"religion=buddhist", QPixmap("://icons/poi/SJJB/png/place_of_worship_buddhist.n.32.png")},
        {"religion=christian", QPixmap("://icons/poi/SJJB/png/place_of_worship_christian.n.32.png")},
        {"religion=hindu", QPixmap("://icons/poi/SJJB/png/place_of_worship_hindu.n.32.png")},
        {"religion=muslim", QPixmap("://icons/poi/SJJB/png/place_of_worship_islamic.n.32.png")},
        {"religion=jain", QPixmap("://icons/poi/SJJB/png/place_of_worship_jain.n.32.png")},
        {"religion=jewish", QPixmap("://icons/poi/SJJB/png/place_of_worship_jewish.n.32.png")},
        {"religion=shinto", QPixmap("://icons/poi/SJJB/png/place_of_worship_shinto.n.32.png")},
        {"religion=sikh", QPixmap("://icons/poi/SJJB/png/place_of_worship_sikh.n.32.png")}
    });
    map["amenity=police"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_police.n.32.png"));
    map["amenity=post_box"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_post_box.n.32.png"), "Post Office");
    map["amenity=post_office"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_post_office.n.32.png"), "Post Office");
    map["amenity=prison"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_prison.n.32.png"));
    map["amenity=pub"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_pub.n.32.png"), "Bar");
    map["amenity=recycling"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_recycling.n.32.png"));
    map["amenity=restaurant"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_restaurant.n.32.png"), "Restaurant",
    {
        {"cuisine=pizza", QPixmap("://icons/poi/SJJB/png/food_pizza.n.32.png")}
    });
    map["amenity=school"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/education_school.n.32.png"), "School");
    map["amenity=shelter"] = CPoiIconCategory(QPixmap("://icons/poi/png/accommodation_shelter_r_n32.png"), "Lodge");
    map["amenity=taxi"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_taxi_rank.n.32.png"));
    map["amenity=telephone"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_telephone.n.32.png"), "Telephone");
    map["amenity=theatre"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_theatre.n.32.png"), "Live Theater");
    map["amenity=toilets"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_toilets.n.32.png"), "Restroom",
    {
        {"wheelchair=yes", QPixmap("://icons/poi/SJJB/png/amenity_toilets_disabled.n.32.png")},
        {"male=yes", QPixmap("://icons/poi/SJJB/png/amenity_toilets_men.n.32.png")},
        {"female=yes", QPixmap("://icons/poi/SJJB/png/amenity_toilets_women.n.32.png")}
    });
    map["amenity=townhall"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_town_hall.n.32.png"), "City Hall");
    map["amenity=university"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/education_university.n.32.png"), "School");
    map["amenity=vending_machine"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_vending_machine.n.32.png"));
    map["amenity=veterinary"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/health_veterinary.n.32.png"));
    map["amenity=water_point"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_drinkingtap.n.32.png"));
    map["amenity=waste_basket"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_waste_bin.n.32.png"));
    map["barrier=block"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_blocks.n.32.png"));
    map["barrier=bollard"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_bollard.n.32.png"));
    map["barrier=cattle_grid"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_cattle_grid.n.32.png"));
    map["barrier=cycle_barrier"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_cycle_barrier.n.32.png"));
    map["barrier=entrance"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_entrance.n.32.png"));
    map["barrier=gate"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_gate.n.32.png"));
    map["barrier=kissing_gate"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_kissing_gate.n.32.png"));
    map["barrier=lift_gate"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_lift_gate.n.32.png"));
    map["barrier=stile"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_stile.n.32.png"));
    map["barrier=toll_booth"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_toll_booth.n.32.png"));
    map["boundary=administrative"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_boundary_administrative.n.32.png"));
    map["entrance=emergency"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_exit.n.32.png"));
    map["entrance=exit"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_exit.n.32.png"));
    map["entrance=main"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_entrance.n.32.png"));
    map["entrance=yes"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_entrance.n.32.png"));
    map["ford=yes"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_ford.n.32.png"));
    map["healthcare=optometrist"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/health_opticians.n.32.png"), "Medical Facility");
    map["highway=bus_stop"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_bus_stop.n.32.png"));
    map["highway=steps"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/barrier_steps.n.32.png"));
    map["historic=archaeological_site"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_archaeological.n.32.png"));
    map["historic=battlefield"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_battlefield.n.32.png"));
    map["historic=castle"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_castle.n.32.png"));
    map["historic=memorial"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_memorial.n.32.png"));
    map["historic=mine"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_mine_abandoned.n.32.png"), "Mine");
    map["historic=monument"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_monument.n.32.png"));
    map["historic=ruins"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_ruin.n.32.png"), "Ghost Town");
    map["historic=wayside_cross"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_wayside_cross.n.32.png"));
    map["historic=wayside_shrine"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_wayside_shrine.n.32.png"));
    map["historic=wreck"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_wreck.n.32.png"), "Shipwreck");
    map["industrial=mine"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_mine.n.32.png"), "Mine",
    {
        {"disused=yes", QPixmap("://icons/poi/SJJB/png/poi_mine_abandoned.n.32.png")}
    });
    map["industrial=port"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_port.n.32.png"));
    map["information=guidepost"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_guidepost.n.32.png"));
    map["landuse=forest"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_coniferous_and_deciduous.n.32.png"), "Forest");
    map["landuse=harbour"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_port.n.32.png"));
    map["landuse=port"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_port.n.32.png"));
    map["landuse=quarry"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_quary.n.32.png"));
    map["landuse=recreation_ground"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_leisure_centre.n.32.png"));
    map["leaf_cycle=deciduous"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_deciduous.n.32.png"), "Forest");
    map["leaf_cycle=evergreen"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_coniferous.n.32.png"), "Forest");
    map["leisure=fitness_centre"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_gym.n.32.png"), "Fitness Center");
    map["leisure=golf_course"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_golf.n.32.png"));
    map["leisure=marina"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_marina.n.32.png"));
    map["leisure=park"] = CPoiIconCategory(QPixmap("://icons/poi/png/amenity_leisure_park_n32.png"));
    map["leisure=picnic_site"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_picnic.n.32.png"), "Picnic Area");
    map["leisure=picnic_table"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_picnic.n.32.png"), "Picnic Area");
    map["leisure=playground"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_playground.n.32.png"));
    map["leisure=playground"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_playground.n.32.png"));
    map["leisure=slipway"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_slipway.n.32.png"), "Boat Ramp");
    map["leisure=sports_centre"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_gymnasium.n.32.png"), "Fitness Center");
    map["leisure=sports_hall"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_gymnasium.n.32.png"), "Fitness Center");
    map["leisure=stadium"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_stadium.n.32.png"));
    map["leisure=swimming_pool"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_swimming_indoor.n.32.png"));
    map["leisure=water_park"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_swimming_outdoor.n.32.png"));
    map["man_made=adit"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_cave.n.32.png"));
    map["man_made=adit"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_mine.n.32.png"), "Mine",
    {
        {"disused=yes", QPixmap("://icons/poi/SJJB/png/poi_mine_abandoned.n.32.png")}
    });
    map["man_made=crane"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_crane.n.32.png"));
    map["man_made=lighthouse"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_lighthouse.n.32.png"));
    map["man_made=mast"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_tower_communications.n.32.png"), "Tall Tower");
    map["man_made=mineshaft"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_mine.n.32.png"), "Mine",
    {
        {"disused=yes", QPixmap("://icons/poi/SJJB/png/poi_mine_abandoned.n.32.png")}
    });
    map["man_made=survey_point"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/amenity_survey_point.n.32.png"));
    map["man_made=tower"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_tower_lookout.n.32.png"), "Small Tower",
    {
        {"tower:type=communication", QPixmap("://icons/poi/SJJB/png/poi_tower_communications.n.32.png")},
        {"tower=communication", QPixmap("://icons/poi/SJJB/png/poi_tower_communications.n.32.png")}
    });
    map["man_made=water_tower"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_tower_water.n.32.png"), "Small Tower");
    //map["man_made=water_tower"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/water_tower.n.32.png"),  "Small Tower");
    map["man_made=water_well"] = CPoiIconCategory(QPixmap("://icons/poi/png/man_made_water_well.png"));
    map["man_made=watermill"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_waterwheel.n.32.png"));
    map["man_made=windmill"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_windmill.n.32.png"));
    map["memorial="] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_memorial.n.32.png"));
    map["military=bunker"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_military_bunker.n.32.png"));
    map["military=range"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_shooting.n.32.png"));
    map["mountain_pass=yes"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_mountain_pass.n.32.png"));
    map["natural=beach"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_beach.n.32.png"), "Beach");
    map["natural=cave_entrance"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_cave.n.32.png"));
    map["natural=peak"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_peak.n.32.png"), "Summit");
    map["natural=scrub"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_scrub.n.32.png"));
    map["natural=spring"] = CPoiIconCategory(QPixmap("://icons/poi/png/water_spring_n32.png"));
    map["natural=tree"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_coniferous_and_deciduous.n.32.png"), "Forest");
    map["natural=volcano"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_peak.n.32.png"));
    map["natural=waterfall"] = CPoiIconCategory(QPixmap("://icons/poi/png/water_waterfall_n32.png"));
    map["natural=wetland"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_swamp.n.32.png"));
    map["natural=wood"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/landuse_coniferous_and_deciduous.n.32.png"), "Forest");
    map["office=estateagent"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_estateagent.n.32.png"));
    map["piste:type=downhill"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_skiing_downhill.n.32.png"));
    map["piste:type=nordic"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_skiing_crosscountry.n.32.png"));
    map["place=city"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_place_city.n.32.png"), "City (Large)");
    map["place=hamlet"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_place_hamlet.n.32.png"), "City (Small)");
    map["place=suburb"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_place_suburb.n.32.png"), "City (Small)");
    map["place=town"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_place_town.n.32.png"), "City (Medium)");
    map["place=village"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_place_village.n.32.png"), "City (Small)");
    map["power=tower"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/poi_tower_power.n.32.png"), "Tall Tower");
    map["public_transport=platform"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_bus_station.n.32.png"), "Ground Transportation",
    {
        {"railway=yes", QPixmap("://icons/poi/SJJB/png/transport_train_station.n.32.png")},
        {"tram=yes", QPixmap("://icons/poi/SJJB/png/transport_tram_stop.n.32.png")},
        {"bus=yes", QPixmap("://icons/poi/SJJB/png/transport_bus_station.n.32.png")},
    });
    map["public_transport=station"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_bus_station.n.32.png"), "Ground Transportation",
    {
        {"railway=yes", QPixmap("://icons/poi/SJJB/png/transport_train_station.n.32.png")},
        {"tram=yes", QPixmap("://icons/poi/SJJB/png/transport_tram_stop.n.32.png")},
        {"bus=yes", QPixmap("://icons/poi/SJJB/png/transport_bus_station.n.32.png")},
    });
    map["public_transport=stop_position"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_bus_station.n.32.png"), "Ground Transportation",
    {
        {"railway=yes", QPixmap("://icons/poi/SJJB/png/transport_train_station.n.32.png")},
        {"tram=yes", QPixmap("://icons/poi/SJJB/png/transport_tram_stop.n.32.png")},
        {"bus=yes", QPixmap("://icons/poi/SJJB/png/transport_bus_station.n.32.png")},
    });
    map["railway:preserved=yes"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_steam_train.n.32.png"));
    map["railway=preserved"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_steam_train.n.32.png"));
    map["railway=station"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_train_station.n.32.png"), "Railway");
    map["railway=tram_stop"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_tram_stop.n.32.png"));
    map["shop=alcohol"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_alcohol.n.32.png"), "Winery");
    map["shop=bakery"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_bakery.n.32.png"));
    map["shop=bicycle"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_bicycle.n.32.png"));
    map["shop=books"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_book.n.32.png"));
    map["shop=butcher"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_butcher.n.32.png"));
    map["shop=car"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_car.n.32.png"));
    map["shop=car_repair"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_car_repair.n.32.png"), "Car Repair");
    map["shop=clothes"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_clothes.n.32.png"));
    map["shop=computer"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_computer.n.32.png"));
    map["shop=confectionery"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_confectionery.n.32.png"));
    map["shop=convenience"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_convenience.n.32.png"), "Convenience Store");
    map["shop=copyshop"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_copyshop.n.32.png"));
    map["shop=craft"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_diy.n.32.png"));
    map["shop=department_store"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_department_store.n.32.png"), "Department Store");
    map["shop=fishing"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_tackle.n.32.png"));
    map["shop=florist"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_florist.n.32.png"));
    map["shop=garden_centre"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_garden_centre.n.32.png"));
    map["shop=gift"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_gift.n.32.png"));
    map["shop=greengrocer"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_greengrocer.n.32.png"));
    map["shop=hairdresser"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_hairdresser.n.32.png"));
    map["shop=hearing_aids"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_hearing_aids.n.32.png"));
    map["shop=hifi"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_hifi.n.32.png"));
    map["shop=jetski"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_jetski.n.32.png"));
    map["shop=jewelry"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_jewelry.n.32.png"));
    map["shop=kiosk"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_kiosk.n.32.png"));
    map["shop=laundry"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_laundrette.n.32.png"));
    map["shop=mobile_phone"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_mobile_phone.n.32.png"));
    map["shop=motorcycle"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_motorcycle.n.32.png"));
    map["shop=music"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_music.n.32.png"));
    map["shop=newsagent"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_newspaper.n.32.png"));
    map["shop=pet"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_pet.n.32.png"));
    map["shop=photo"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_photo.n.32.png"));
    map["shop=seafood"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_fish.n.32.png"));
    map["shop=supermarket"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_supermarket.n.32.png"));
    map["shop=tobacco"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_tobacco.n.32.png"));
    map["shop=toys"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_toys.n.32.png"));
    map["shop=video"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/shopping_video_rental.n.32.png"));
    map["sport=archery"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_archery.n.32.png"));
    map["sport=baseball"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_baseball.n.32.png"));
    map["sport=canoe"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_canoe.n.32.png"));
    map["sport=cliff_diving"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_diving.n.32.png"));
    map["sport=climbing"] = CPoiIconCategory(QPixmap("://icons/poi/png/sport_rock_climbing_n32.png"));
    map["sport=cricket"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_cricket.n.32.png"));
    map["sport=horse_reacing"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_horse_racing.n.32.png"));
    map["sport=ice_skating"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_iceskating.n.32.png"));
    map["sport=miniature_golf"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_minature_golf.n.32.png"));
    map["sport=motor"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_motorracing.n.32.png"));
    map["sport=sailing"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_sailing.n.32.png"));
    map["sport=scuba_diving"] = CPoiIconCategory(QPixmap("://icons/poi/png/sport_scuba_diving_n32.png"));
    map["sport=shooting"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_shooting.n.32.png"));
    map["sport=skiing"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_skiing_downhill.n.32.png"));
    map["sport=snooker"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_snooker.n.32.png"));
    map["sport=soccer"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_soccer.n.32.png"));
    map["sport=surfing"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_windsurfing.n.32.png"));
    map["sport=swimming"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_swimming_outdoor.n.32.png"), "Swimming Area",
    {
        {"building=yes", QPixmap("://icons/poi/SJJB/png/sport_swimming_indoor.n.32.png")}
    });
    map["sport=tennis"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/sport_tennis.n.32.png"));
    map["station=subway"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/transport_subway.n.32.png"), "Ground Transportation");
    map["tourism=alpine_hut"] = CPoiIconCategory(QPixmap("://icons/poi/png/accommodation_alpinehut_r_n32.png"), "Lodge");
    map["tourism=attraction"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_attraction.n.32.png"));
    map["tourism=camp_site"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_camping.n.32.png"), "Campground");
    map["tourism=caravan_site"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_caravan_park.n.32.png"), "RV Park");
    map["tourism=chalet"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_chalet.n.32.png"), "Lodging");
    map["tourism=gallery"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_art_gallery.n.32.png"));
    map["tourism=guest_house"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_bed_and_breakfast.n.32.png"), "Lodging");
    map["tourism=hostel"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_hostel.n.32.png"), "Lodging");
    map["tourism=hotel"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_hotel.n.32.png"), "Lodging");
    map["tourism=information"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_information.n.32.png"), "Information",
    {
        {"information=board", QPixmap("://icons/poi/SJJB/png/tourist_map.n.32.png")},
        {"information=map", QPixmap("://icons/poi/SJJB/png/tourist_map.n.32.png")}
    });
    map["tourism=motel"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/accommodation_motel.n.32.png"), "Lodging");
    map["tourism=museum"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_museum.n.32.png"), "Museum");
    map["tourism=picnic_site"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_picnic.n.32.png"), "Picnic Area");
    map["tourism=theme_park"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_theme_park.n.32.png"));
    map["tourism=viewpoint"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_view_point.n.32.png"), "Scenic Area");
    map["tourism=wilderness_hut"] = CPoiIconCategory(QPixmap("://icons/poi/png/accommodation_wilderness_hut_r_n32.png"), "Lodge");
    map["tourism=zoo"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/tourist_zoo.n.32.png"), "Zoo");
    map["waterway=dam"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/water_dam.n.32.png"), "Dam");
    map["waterway=water_point"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/food_drinkingtap.n.32.png"));
    map["waterway=waterfall"] = CPoiIconCategory(QPixmap("://icons/poi/png/water_waterfall_n32.png"));
    map["waterway=weir"] = CPoiIconCategory(QPixmap("://icons/poi/SJJB/png/water_weir.n.32.png"), "Dam");

    return map;
}
