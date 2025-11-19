package com.maivantien.lab302_new.service;
import java.util.List;
import com.maivantien.lab302_new.entity.Product;
public interface ProductService {
    Product createProduct(Product product);
    Product getProductById(Long productId);
    List<Product> getAllProducts();
    Product updateProduct(Product product);
    void deleteProduct(Long productId);
}