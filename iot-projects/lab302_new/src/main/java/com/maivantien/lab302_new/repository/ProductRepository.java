package com.maivantien.lab302_new.repository;
import org.springframework.data.jpa.repository.JpaRepository;
import com.maivantien.lab302_new.entity.Product;
public interface ProductRepository extends JpaRepository<Product, Long> {
}
