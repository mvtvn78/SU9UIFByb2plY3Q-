package com.maivantien.lab304_new.repository;
import org.springframework.data.jpa.repository.JpaRepository;
import com.maivantien.lab304_new.entity.Product;
public interface ProductRepository extends JpaRepository<Product, Long> {
}