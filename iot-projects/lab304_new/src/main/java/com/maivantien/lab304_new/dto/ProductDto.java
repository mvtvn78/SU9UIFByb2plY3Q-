package com.maivantien.lab304_new.dto;

import lombok.AllArgsConstructor;
import lombok.Getter;
import lombok.Setter;

@AllArgsConstructor
@Setter
@Getter
public class ProductDto {
    private String title;
    private String description;
    private String photo;
    private double price;
    private Long category_id;
}
